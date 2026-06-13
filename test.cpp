#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN // doctest – фреймворк тестирования
#include "doctest.h"
#include "functions.h"
#include <fstream>

TEST_CASE("split разделяет строку по запятой") {
    std::vector<std::string> result = split("a,b,c", ',');
    std::vector<std::string> expected = {"a", "b", "c"};
    CHECK(result == expected);
}

TEST_CASE("split разделяет строку по табуляции") {
    std::vector<std::string> result = split("x\ty", '\t');
    std::vector<std::string> expected = {"x", "y"};
    CHECK(result == expected);
}

TEST_CASE("detect_delimiter находит табуляцию") {
    CHECK(detect_delimiter("a\tb") == '\t');
}

TEST_CASE("detect_delimiter возвращает запятую, если табуляции нет") {
    CHECK(detect_delimiter("a,b") == ',');
}

TEST_CASE("read_csv корректно читает данные и метки") {
    std::ofstream tmp("test_data.csv");
    tmp << "Temperature,Profit\n";
    tmp << "10,20\n";
    tmp << "15,30\n";
    tmp.close();

    std::vector<double> x, y;
    std::string x_label, y_label;
    bool ok = read_csv("test_data.csv", 1, 0, 1, x, y, x_label, y_label);
    CHECK(ok == true);
    CHECK(x.size() == 2);
    CHECK(y.size() == 2);
    CHECK(x[0] == 10.0);
    CHECK(y[0] == 20.0);
    CHECK(x[1] == 15.0);
    CHECK(y[1] == 30.0);
    CHECK(x_label == "Temperature");
    CHECK(y_label == "Profit");

    std::remove("test_data.csv");
}

TEST_CASE("read_csv возвращает false при нехватке данных") {
    std::ofstream tmp("small.csv");
    tmp << "X,Y\n";
    tmp << "1,2\n";
    tmp.close();

    std::vector<double> x, y;
    std::string xl, yl;
    bool ok = read_csv("small.csv", 1, 0, 1, x, y, xl, yl);
    CHECK(ok == false);
    std::remove("small.csv");
}

TEST_CASE("read_csv пропускает нечисловые строки") {
    std::ofstream tmp("bad.csv");
    tmp << "X,Y\n";
    tmp << "abc,10\n";
    tmp << "5,20\n";
    tmp << "8,15\n";
    tmp.close();

    std::vector<double> x, y;
    std::string xl, yl;
    bool ok = read_csv("bad.csv", 1, 0, 1, x, y, xl, yl);
    CHECK(ok == true);
    CHECK(x.size() == 2);
    CHECK(x[0] == 5.0);
    CHECK(y[0] == 20.0);
    CHECK(x[1] == 8.0);
    CHECK(y[1] == 15.0);
    std::remove("bad.csv");
}