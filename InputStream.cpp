#include "InputStream.hpp"
#include "scripting_impl.hpp"

SCRIPTABLE_MAP_BEGIN(InputStream, Inanity.InputStream);
SCRIPTABLE_MAP_END();

bigsize_t InputStream::Skip(bigsize_t size)
{
	char buffer[0x1000];
	bigsize_t skipped = 0;
	while(size)
	{
		size_t read = Read(buffer, std::min<bigsize_t>(size, sizeof(buffer)));
		if(!read)
			break;
		size -= read;
		skipped += read;
	}
	return skipped;
}
