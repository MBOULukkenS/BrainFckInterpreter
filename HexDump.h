//
// Created by Stijn on 22/04/2020.
//

#ifndef BRAINFCKINTERPRETER_HEXDUMP_H
#define BRAINFCKINTERPRETER_HEXDUMP_H

#include <ios>
#include <iomanip>
#include <sstream>
#include <vector>

namespace HexDump {

    using std::endl;
    using std::hex;
    using std::isprint;
    using std::left;
    using std::ostringstream;
    using std::setfill;
    using std::setw;
    using std::size_t;
    using std::vector;

    // In the templates below, the type parameters must satisfy these constraints:
    //
    // - Stream: something like std::ostream or std::istream
    //
    // - InputStream: something like std::istream
    //
    // - OutputStream: something like std::ostream
    //
    // - ByteSequence: a collection of chars that can be iterated over, like std::vector<char>

    const size_t BytesPerLine = 16;

    struct MemBuf : std::streambuf
    {
        MemBuf(char* begin, char* end) 
        {
            this->setg(begin, begin, end);
        }
    };
    
    struct MemStream : std::istream
    {
        MemStream(MemBuf *buf) : std::istream(buf)
        {}

        MemStream(char* begin, char* end) : MemStream(new MemBuf(begin, end))
        {}
    };

    // Saves original formatting state for a stream and
    // restores that state before going out of scope
    template<typename Stream>
    class SavedState
    {
    public:
        SavedState(Stream& s)
                : stream(s), oldFlags(s.flags()), oldFill(s.fill())
        {}

        ~SavedState() { stream.flags(oldFlags); stream.fill(oldFill); }

        SavedState(const SavedState&) = delete;
        void operator=(const SavedState&) = delete;

    private:
        Stream& stream;
        decltype(stream.flags()) oldFlags;
        decltype(stream.fill()) oldFill;
    };

    // Dump a sequence of bytes as hex with spaces between; e.g., "ab cd ef 01 "
    template<typename OutputStream, typename ByteSequence>
    void dumpBytesAsHex(OutputStream& output, const ByteSequence& bytes)
    {
        SavedState<OutputStream> savedState{ output };

        output << hex << setfill('0');

        for (auto byte: bytes) {
            unsigned widenedUIntValue = static_cast<unsigned char>(byte);
            output << setw(2) << widenedUIntValue << " ";
        }
    }

    // Dump a sequence of bytes as ASCII characters,
    // substituting '.' for non-printing characters
    template<typename OutputStream, typename ByteSequence>
    void dumpBytesAsText(OutputStream& output, const ByteSequence& bytes)
    {
        for (auto byte: bytes)
            output.put(isprint(byte) ? byte : '.');
    }

    // Dump a sequence of bytes in side-by-side hex and text formats
    template<typename OutputStream, typename ByteSequence>
    void dumpHexLine(OutputStream& output, const ByteSequence& bytes)
    {
        SavedState<OutputStream> savedState{ output };

        ostringstream hexStream;
        dumpBytesAsHex(hexStream, bytes);
        const auto HexOutputWidth = BytesPerLine * 3 + 1;
        output << setw(HexOutputWidth) << left << hexStream.str();

        dumpBytesAsText(output, bytes);

        output << endl;
    }

    // Dump a sequence of bytes in side-by-side hex and text formats,
    // prefixed with a hex offset
    template<typename OutputStream, typename ByteSequence>
    void dumpHexLine(OutputStream& output, size_t offset, const ByteSequence& bytes)
    {
        {
            SavedState<OutputStream> savedState{ output };
            output << setw(8) << setfill('0') << hex
                   << offset << "  ";
        }

        dumpHexLine(output, bytes);
    }

    // Dump bytes from input stream in side-by-side hex and text formats
    template<typename OutputStream, typename InputStream>
    void DumpStream(OutputStream& output, InputStream& input)
    {
        vector<char> bytesToDump;
        bytesToDump.reserve(BytesPerLine);

        size_t offset = 0;

        char byte;
        while (input.get(byte)) {
            bytesToDump.push_back(byte);

            if (bytesToDump.size() == BytesPerLine) {
                dumpHexLine(output, offset, bytesToDump);
                bytesToDump.clear();
                offset += BytesPerLine;
            }
        }

        if (!bytesToDump.empty())
            dumpHexLine(output, offset, bytesToDump);
    }

    template<typename OutputStream>
    void DumpCharArray(OutputStream &output, char *array, size_t length)
    {
        MemStream memStream = MemStream(array, array + length);
        DumpStream(output, memStream);
    }
} // namespace HexDump

#endif //BRAINFCKINTERPRETER_HEXDUMP_H
