
#include "VerdiMinMaxOctree.h"
#include "VerdiMath.h"
#include "VerdiException.h"

namespace Verdi {

	MinMaxOctree::Node::Node()
	{

	}

	MinMaxOctree::Node::Node(Node* p)
	{
		parent = p;
		for(int i = 0; i < 8; ++i)
			children[i] = NULL;

		max = Verdi::Math::INFINITY_NEGATIVE;
		min = Verdi::Math::INFINITY_POSITIVE;

		window.x = 0;
		window.y = 0;
		window.z = 0;

		window.width = 0;
		window.height = 0;
		window.depth = 0;

		isLeaf = false;
	}

	MinMaxOctree::Node::~Node()
	{

	}

	MinMaxOctree::MinMaxOctree()
	{
		mNodes.clear();
		mLeafNodes.clear();

		clear();
	}

	MinMaxOctree::~MinMaxOctree()
	{
		NodeList::iterator i = mNodes.begin();
		for( ; i != mNodes.end(); ++i)
		{
			Node* n = (*i);
			delete n;
		}

		mNodes.clear();
		mLeafNodes.clear();
	}

	void MinMaxOctree::clear()
	{
		NodeList::iterator i = mNodes.begin();
		for( ; i != mNodes.end(); ++i)
		{
			Node* n = (*i);
			delete n;
		}

		mNodes.clear();
		mLeafNodes.clear();

		mpRootNode = new Node(NULL);
		mNodes.push_back(mpRootNode);

		for(int i = 0; i < MAPPING_TABLE_DIM; ++i) {
			for(int j = 0; j < MAPPING_TABLE_DIM; ++j) {
				mMappingTable[i][j] = 0;
			}
		}
	}

	void MinMaxOctree::_buildTree(Node* parent, int depth)
	{
		if(depth-1 == 0)
		{
			parent->isLeaf = true;
			mLeafNodes.push_back(parent);
			return;
		}

		int hw0 = parent->window.width / 2;
		int hh0 = parent->window.height / 2;
		int hd0 = parent->window.depth / 2;

		int hw1 = (int)ceilf( (float)parent->window.width / 2.0f );
		int hh1 = (int)ceilf( (float)parent->window.height / 2.0f );
		int hd1 = (int)ceilf( (float)parent->window.depth / 2.0f );

		for(int i = 0; i < 8; ++i)
		{
			Node* newNode = new Node(parent);

			newNode->window.x = parent->window.x;
			newNode->window.y = parent->window.y;
			newNode->window.z = parent->window.z;

			newNode->window.width = hw0;
			newNode->window.height = hh0;
			newNode->window.depth = hd0;

			if( i & 0x01 )
			{
				newNode->window.x += hw0;
				newNode->window.width = hw1;
			}

			if( i & 0x02 )
			{
				newNode->window.y += hh0;
				newNode->window.height = hh1;
			}

			if( i & 0x04 )
			{
				newNode->window.z += hd0;
				newNode->window.depth = hd1;
			}

			parent->children[i] = newNode;
			mNodes.push_back(newNode);
			_buildTree(newNode,depth-1);
		}
	}

	void MinMaxOctree::_propagateMinMaxValues(Node* parent)
	{
		for(int i = 0; i < 8; ++i)
		{
			if(parent->children[i] == NULL)
				return;

			_propagateMinMaxValues(parent->children[i]);
		}

		float minVal = Math::INFINITY_POSITIVE;
		float maxVal = Math::INFINITY_NEGATIVE;

		for(int i = 0; i < 8; ++i)
		{
			minVal = Math::minimum<float>(minVal,parent->children[i]->min);
			maxVal = Math::maximum<float>(maxVal,parent->children[i]->max);
		}

		parent->min = minVal;
		parent->max = maxVal;
	}

	void MinMaxOctree::create(const DataSet* valueDataSet, uint32_t valueIndex, uint32_t depth)
	{
		if(depth < 2 || depth > 8)
		{
			VERDI_THROW(Verdi::ParameterInvalidException,"Depth must be between 2 and 8",0);
		}

		mpRootNode->window.x = 0;
		mpRootNode->window.y = 0;
		mpRootNode->window.z = 0;

		mpRootNode->window.width = valueDataSet->getWidth();
		mpRootNode->window.height = valueDataSet->getHeight();
		mpRootNode->window.depth = valueDataSet->getDepth();

		_buildTree(mpRootNode,depth);

		NodeList::iterator i = mLeafNodes.begin();
		for( ; i != mLeafNodes.end(); ++i)
		{
			Node* n = (*i);
			DataSet::computeMinMaxValues( n->window,valueDataSet,valueIndex, &(n->min),&(n->max));
		}

		_propagateMinMaxValues(mpRootNode);
	}

	void MinMaxOctree::create(const DataSet* valueDataSet, uint32_t valueIndex, 
		const DataSet* gradMagDataSet, uint32_t gradMagIndex, const TransferFunction* transferFunction,
		uint32_t depth)
	{
		if(depth < 2 || depth > 8)
		{
			VERDI_THROW(Verdi::ParameterInvalidException,"Depth must be between 2 and 8",0);
		}

		mpRootNode->window.x = 0;
		mpRootNode->window.y = 0;
		mpRootNode->window.z = 0;

		mpRootNode->window.width = valueDataSet->getWidth();
		mpRootNode->window.height = valueDataSet->getHeight();
		mpRootNode->window.depth = valueDataSet->getDepth();

		_buildTree(mpRootNode,depth);
	
		
		int sz = mLeafNodes.size();

		#pragma omp parallel for
		for(int i = 0; i < sz; ++i)
		{
			Node* n = mLeafNodes[i];
			DataSet::computeMinMaxValues( n->window,valueDataSet,valueIndex,
				gradMagDataSet,gradMagIndex,transferFunction,&(n->min),&(n->max));
		}

		_propagateMinMaxValues(mpRootNode);
	}

	void MinMaxOctree::updateMappingTable(const TransferFunction* func)
	{
		const float dx = 1.0f/(float)(func->getWidth());

		for(int i = 0; i < MAPPING_TABLE_DIM; ++i) {
			for(int j = 0; j < MAPPING_TABLE_DIM; ++j) {

				float fi = (float) i / (float)(MAPPING_TABLE_DIM-1);
				float fj = (float) j / (float)(MAPPING_TABLE_DIM-1);

				float sx = Verdi::Math::minimum(fi,fj);
				float ex = Verdi::Math::maximum(fi,fj);

				uint8_t vis = 0;

				for(float x = sx; x <= ex; x += dx) {
					float f = func->sample(x,0.5f).w;

					if(f > 0.001) {
						vis = 1;
						break;
					}
				}

				mMappingTable[i][j] = vis;
			}
		}
	}

	bool MinMaxOctree::isVisible(const Node* node) const
	{
		return sampleMappingTable(node->min, node->max);
	}

	bool MinMaxOctree::sampleMappingTable(float minv, float maxv) const
	{	
		int sx = (int)floorf(minv * (float)(MAPPING_TABLE_DIM-1) );
		int sy = (int)ceilf(maxv * (float)(MAPPING_TABLE_DIM-1) );

		if(sx >= MAPPING_TABLE_DIM)
			sx = MAPPING_TABLE_DIM-1;

		if(sy >= MAPPING_TABLE_DIM)
			sy = MAPPING_TABLE_DIM-1;

		return static_cast<bool>(mMappingTable[sx][sy] != 0);
	}
}