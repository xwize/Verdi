
#ifndef __Verdi_Exception_H__
#define __Verdi_Exception_H__

#include "VerdiPrerequisites.h"

namespace Verdi {

	/** The base class for all exceptions
		used throughout the framework.
	*/
	class Exception
	{
	protected:
		int mCode;
		int mLine;
		String mDescription;
		String mSourceFile;
	private:
		Exception();
	public:
		Exception(const String& description, int code = 0, int line = -1, const String& sourceFile = "")
			: mCode(code), mDescription(description), mLine(line), mSourceFile(sourceFile) {}
		Exception(const Exception& e);

		/** Get the description of the exception.
		*/
		virtual const String& getDescription() const { return mDescription; }

		/** Get the source file name where the exception was thrown.
		*/
		virtual const String& getSourceFile() const { return mSourceFile; }

		/** Get the source file line where the exception was thrown.
		*/
		virtual int getLine() const { return mLine; }

		/** Get the thrower-assigned code value.
		*/
		virtual int getCode() const { return mCode; }
	};

	class FileNotFoundException : public Exception
	{
	protected:
	public:
		FileNotFoundException(const String& description, int code = 0, int line = -1, const String& sourceFile = "")
			: Exception(description,code,line,sourceFile) {}
	};

	class FileLoadFailedException : public Exception
	{
	protected:
	public:
		FileLoadFailedException(const String& description, int code = 0, int line = -1, const String& sourceFile = "")
			: Exception(description,code,line,sourceFile) {}
	};

	class ParameterInvalidException : public Exception
	{
	protected:
	public:
		ParameterInvalidException(const String& description, int code = 0, int line = -1, const String& sourceFile = "")
			: Exception(description,code,line,sourceFile) {}
	};

	class StateInvalidException : public Exception
	{
	protected:
	public:
		StateInvalidException(const String& description, int code = 0, int line = -1, const String& sourceFile = "")
			: Exception(description,code,line,sourceFile) {}
	};

	class ObjectNotFoundException : public Exception
	{
	protected:
	public:
		ObjectNotFoundException(const String& description, int code = 0, int line = -1, const String& sourceFile = "")
			: Exception(description,code,line,sourceFile) {}
	};

	class ObjectDuplicateFoundException : public Exception
	{
	protected:
	public:
		ObjectDuplicateFoundException(const String& description, int code = 0, int line = -1, const String& sourceFile = "")
			: Exception(description,code,line,sourceFile) {}
	};

	class HardwareFeatureUnsupportedException : public Exception
	{
	protected:
	public:
		HardwareFeatureUnsupportedException(const String& description, int code = 0, int line = -1, const String& sourceFile = "")
			: Exception(description,code,line,sourceFile) {}
	};


#ifndef VERDI_THROW
#define VERDI_THROW(className,description,code) \
	throw className ( description, code, __LINE__, __FILE__ )
#endif

}

#endif // __Verdi_Exception_H__