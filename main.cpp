#include <iostream>
#include <cstring>

int bytes_used = 0;
int bytes_copied = 0;
void* xmemcpy(void* destination, const void* src, size_t size)
{
    void* result = memcpy(destination, src, size);

    bytes_copied += size;
    return result;
}

void* xmalloc(size_t size)
{
    void* result = malloc(size);

    bytes_used += size;

    return result;
}

#define memcpy xmemcpy
#define malloc xmalloc

#define DPRINT(s) \
    std::cout << s << std::flush \

#define PRINTLN(s) \
    std::cout << s << std::endl

class string {
private:
    char * data;
    int length;


public:
    string() : string("")
    {

    }
    // constructor
    string(const char* p)
    {
        length = strlen(p);
        data = (char*)malloc(length+1);
        memcpy(data, p, length+1);
        DPRINT("Construct (copy) from \"" << p << "\"\n");
    }


    // move constructor
    // Can safely nullify other rvalue.
    string(string&& other) noexcept
    {
        length = other.length;
        data = other.data;
        other.data = nullptr;
        other.length = 0;

        DPRINT("Move from \"" << data << "\"\n");
    }

    // copy constructor
    string(const string& other)
    {
        length = other.length;
        data = (char*)malloc(length+1);
        memcpy(data, other.data, length+1);
        DPRINT("Copy from \"" << other.data << "\"\n");
    }


    // assignment operator
    string& operator=(string that)
    {
        length = that.length;
        std::swap(data, that.data);
        DPRINT("Assign from \"" << data << "\"\n");
        return *this;
    }

    ~string()
    {
        if (data != nullptr)
        {
            DPRINT("Free \"" << data << "\"\n");
            free(data);
        }
    }

    /* 1st variation for first parameter being lvalue or rvalue.
     * If first param is rvalue, we can safely append to that (instead of copying bytes).
     * 2nd variation if 2nd parameter is string or char array.
     * We don't need to copy to separate string if char array, we can just append directly.
     */
    friend string operator+(const string&, const string&);
    friend string operator+(string&&, const string&);
    friend string operator+(const string&, const char*);
    friend string operator+(string&&, const char*);

    friend std::ostream& operator<<(std::ostream& os, const string& s);
};

string operator+(const string& self, const string& other)
{
    string result(self);

    size_t new_length = result.length + other.length + 1;
    result.data = (char *)realloc(result.data, new_length);
    memcpy(result.data+result.length, other.data, other.length+1);
    result.length += other.length;

    DPRINT("Concatenate to \"" << result.data << "\"\n");
    return result;
}

string operator+(string&& self, const string& other)
{
    string result = std::move(self);

    size_t new_length = result.length + other.length + 1;
    result.data = (char *)realloc(result.data, new_length);
    memcpy(result.data+result.length, other.data, other.length+1);
    result.length += other.length;

    DPRINT("Concatenate to \"" << result.data << "\"\n");
    return result;
}

string operator+(const string& self, const char* other)
{
    string result(self);
    int other_length = strlen(other);

    size_t new_length = result.length + other_length + 1;
    result.data = (char *)realloc(result.data, new_length);
    memcpy(result.data+result.length, other, other_length+1);
    result.length += other_length;

    DPRINT("Concatenate to \"" << result.data << "\"\n");
    return result;
}

string operator+(string&& self, const char* other)
{
    string result = std::move(self);
    int other_length = strlen(other);

    size_t new_length = result.length + other_length + 1;
    result.data = (char *)realloc(result.data, new_length);
    memcpy(result.data+result.length, other, other_length+1);
    result.length += other_length;

    DPRINT("Concatenate to \"" << result.data << "\"\n");
    return result;
}

std::ostream& operator<<(std::ostream& os, const string& s)
{
    os << s.data;
    return os;
}

int main() {
    DPRINT("Init a: "); string a = "Hello";
    DPRINT("Init b: "); string b = "world";
    DPRINT("Strings initialized\n\n");

    DPRINT("Performing assignment:\n");
    string result = a + ", " + (b + "!");
    PRINTLN(result);

    DPRINT("bytes used: " << bytes_used << "\nbytes copied: " << bytes_copied << "\n");
    return 0;
}