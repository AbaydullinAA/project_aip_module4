#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "functions.h"
#include <fstream>
#include <stdexcept>

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

TEST_CASE("split возвращает пустой вектор для пустой строки") {
    std::vector<std::string> result = split("", ',');
    CHECK(result.empty());
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
    REQUIRE_NOTHROW(read_csv("test_data.csv", 1, 0, 1, x, y, x_label, y_label));
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

TEST_CASE("read_csv выбрасывает исключение при нехватке данных") {
    std::ofstream tmp("small.csv");
    tmp << "X,Y\n";
    tmp << "1,2\n";
    tmp.close();

    std::vector<double> x, y;
    std::string xl, yl;
    CHECK_THROWS_AS(read_csv("small.csv", 1, 0, 1, x, y, xl, yl), std::runtime_error);
    std::remove("small.csv");
}

TEST_CASE("read_csv выбрасывает исключение при отсутствии файла") {
    std::vector<double> x, y;
    std::string xl, yl;
    CHECK_THROWS_AS(read_csv("nonexistent.csv", 0, 0, 1, x, y, xl, yl), std::runtime_error);
}

TEST_CASE("write_plot_files создаёт файлы") {
    std::vector<double> x = {1.0, 2.0, 3.0};
    std::vector<double> y = {2.0, 4.0, 6.0};
    double intercept = 0.0;
    double slope = 2.0;
    int N = 10;
    std::string x_label = "X";
    std::string y_label = "Y";

    REQUIRE_NOTHROW(write_plot_files(x, y, intercept, slope, N, x_label, y_label));

    CHECK(std::ifstream("data_points.dat").good());
    CHECK(std::ifstream("fit_line.dat").good());
    CHECK(std::ifstream("plot.gp").good());

    std::remove("data_points.dat");
    std::remove("fit_line.dat");
    std::remove("plot.gp");
}

TEST_CASE("write_plot_files выбрасывает исключение при N_points < 2") {
    std::vector<double> x = {1.0, 2.0};
    std::vector<double> y = {2.0, 4.0};
    double intercept = 0.0;
    double slope = 1.0;
    int N = 1;
    std::string x_label = "X", y_label = "Y";
    CHECK_THROWS_AS(write_plot_files(x, y, intercept, slope, N, x_label, y_label), std::runtime_error);
}

TEST_CASE("compute_linear_fit вычисляет коэффициенты") {
    std::vector<double> x = {1.0, 2.0, 3.0};
    std::vector<double> y = {2.0, 4.0, 6.0};
    double intercept, slope, r;
    REQUIRE_NOTHROW(compute_linear_fit(x, y, intercept, slope, r));
    CHECK(slope == doctest::Approx(2.0));
    CHECK(intercept == doctest::Approx(0.0));
    CHECK(r == doctest::Approx(1.0));
}

TEST_CASE("compute_linear_fit выбрасывает исключение при n < 2") {
    std::vector<double> x = {1.0};
    std::vector<double> y = {2.0};
    double intercept, slope, r;
    CHECK_THROWS_AS(compute_linear_fit(x, y, intercept, slope, r), std::runtime_error);
}

TEST_CASE("validate_inputs не выбрасывает исключений при корректных параметрах") {
    REQUIRE_NOTHROW(validate_inputs(1, 0, 1, 50));
}

TEST_CASE("validate_inputs выбрасывает исключение при отрицательном skip_rows") {
    CHECK_THROWS_AS(validate_inputs(-1, 0, 1, 50), std::invalid_argument);
}

TEST_CASE("validate_inputs выбрасывает исключение при отрицательном col_x") {
    CHECK_THROWS_AS(validate_inputs(0, -1, 1, 50), std::invalid_argument);
}

TEST_CASE("validate_inputs выбрасывает исключение при отрицательном col_y") {
    CHECK_THROWS_AS(validate_inputs(0, 0, -1, 50), std::invalid_argument);
}

TEST_CASE("validate_inputs выбрасывает исключение при col_x == col_y") {
    CHECK_THROWS_AS(validate_inputs(0, 1, 1, 50), std::invalid_argument);
}

TEST_CASE("validate_inputs выбрасывает исключение при N_points < 2") {
    CHECK_THROWS_AS(validate_inputs(0, 0, 1, 1), std::invalid_argument);
}

TEST_CASE("print_data_statistics корректно работает") {
    std::vector<double> x = {1.0, 2.0, 3.0};
    std::vector<double> y = {2.0, 4.0, 6.0};
    CHECK_NOTHROW(print_data_statistics(x, y));
}

TEST_CASE("save_report успешно создает файл") {
    REQUIRE_NOTHROW(save_report("test_report.txt", 0.0, 2.0, 1.0));
    CHECK(std::ifstream("test_report.txt").good());
    std::remove("test_report.txt");
}