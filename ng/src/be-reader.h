#pragma once

class be_reader {
public:
    be_reader(std::vector<uint8_t> &data_, int i_)
        : data(data_)
        , i(0)
    {
        reset(i_);
    }

    be_reader(std::vector<uint8_t> &data_)
        : data(data_)
        , i(0)
    {
        reset(0);
    }

    void reset(int to) {
        assert(to < data.size());
        i = to;
    }

    uint8_t read_8() {
        assert(i < data.size());
        return data[i++];
    }

    uint16_t read_16() {
        return (read_8() << 8) + read_8();
    }

    uint32_t read_32() {
        return (read_16() << 16) + read_16();
    }

private:
    std::vector<uint8_t> &data;
    int i;
};
