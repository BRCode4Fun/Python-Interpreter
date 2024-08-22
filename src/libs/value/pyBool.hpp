#include "./pyInt.hpp"

class PyBool : public PyInt {

public:
    explicit PyBool(bool);

    inline bool is_bool_type() const override { return true; }

    bool getBool() const;

private:
    void registerMethods() override;
    void deleteData() override {}
};