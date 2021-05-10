
#ifndef __Verdi_Min_Max_Octree_H__
#define __Verdi_Min_Max_Octree_H__

#include "VerdiPrerequisites.h"
#include "VerdiDataSet.h"
#include "VerdiTransferFunction.h"

namespace Verdi {

	/** This class is an octree implementation that
		stores the min and max values of a pre-classified
		data set to a particular octree depth. This is can be used in
		an empty space leaping algorithm to improve
		rendering performance.
	*/
	class MinMaxOctree
	{
	public:
		/** An octree node implementation.
		*/
		class Node
		{
		public:
			Node* parent;
			Node* children[8];
			float max, min;
			DataSet::Window window;
			bool isLeaf;
		private:
			/** Private default constructor.
			*/
			Node();
		public:

			/** Parameterised constructor.
			*/
			Node(Node* parent);

			/** Default destructor.
			*/
			~Node();
		};

	public:
		// Typedef for NodeList
		typedef std::vector<Node*> NodeList;
	protected:
		void _buildTree(Node* parent, int depth);
		void _propagateMinMaxValues(Node* parent);
		static const int MAPPING_TABLE_DIM = 128;
		uint8_t mMappingTable[MAPPING_TABLE_DIM][MAPPING_TABLE_DIM];
		NodeList mNodes;
		NodeList mLeafNodes;
		Node* mpRootNode;
	public:

		/** Default constructor.
		*/
		MinMaxOctree();

		/** Default destructor.
		*/
		~MinMaxOctree();

		/** Clear the tree to just 
			an empty root node.
		*/
		void clear();

		/** Create the tree from the data set alone.
		*/
		void create(const DataSet* valueDataSet, uint32_t valueIndex, uint32_t depth);

		/** Create the tree for a specific
			data set and transfer function.
		*/
		void create(const DataSet* valueDataSet, uint32_t valueIndex, 
			const DataSet* gradMagDataSet, uint32_t gradMagIndex, const TransferFunction* transferFunction,
			uint32_t depth); 

		/** Sample the mapping table at the 
			specified coordinates.
		*/
		bool sampleMappingTable(float minv, float maxv) const;

		/** Updates the internal mapping table from a
			transfer function.
		*/
		void updateMappingTable(const TransferFunction* func);

		/** Returns true if a node is visible.
		*/
		bool isVisible(const Node* node) const;

		/** Get the root node.
		*/
		Node* getRootNode() const { return mpRootNode; }
	};
}

#endif // __Verdi_Min_Max_Octree_H__