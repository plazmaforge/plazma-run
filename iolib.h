
char* readBytes(const char* fileName);

char* readBytes(const char* fileName, size_t& size);

void writeBytes(const char* fileName, char* data, size_t& size);

void dumpFile(const char* fileName);

void dumpFile(const char* fileName, size_t& size);
