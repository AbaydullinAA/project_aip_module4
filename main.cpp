#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>

extern "C" {
#include <gsl/gsl_fit.h>
#include <gsl/gsl_statistics_double.h>
}

#include "sciplot/sciplot.hpp"

std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
        tokens.push_back(token);
    return tokens;
}

char detect_delimiter(const std::string &line) {
    if (line.find('\t') != std::string::npos) return '\t';
    return ',';
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        std::cerr << "Использование: " << argv[0]
                  << " <csv-файл> <пропуск строк> <столбец X> <столбец Y> <точек на прямой>\n";
        return 1;
    }

    std::string filename = argv[1];
    unsigned int skip_rows = std::stoi(argv[2]);
    unsigned int col_x = std::stoi(argv[3]);
    unsigned int col_y = std::stoi(argv[4]);
    unsigned int N_points = std::stoi(argv[5]);

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла: " << filename << std::endl;
        return 1;
    }

    // Автоматические подписи осей из заголовка
    std::string x_label = "X", y_label = "Y";
    std::string header_line;
    if (std::getline(file, header_line)) {
        file.clear();
        file.seekg(0);
        char h_delim = detect_delimiter(header_line);
        auto header_tokens = split(header_line, h_delim);
        if (col_x < header_tokens.size())
            x_label = header_tokens[col_x];
        if (col_y < header_tokens.size())
            y_label = header_tokens[col_y];
    }

    std::vector<double> x, y;
    std::string line;
    unsigned int row = 0;
    char delimiter = 0;

    while (std::getline(file, line)) {
        if (row < skip_rows) {
            ++row;
            continue;
        }
        if (delimiter == 0)
            delimiter = detect_delimiter(line);

        auto tokens = split(line, delimiter);
        if (tokens.size() <= std::max(col_x, col_y)) {
            ++row;
            continue;
        }
        try {
            double xv = std::stod(tokens[col_x]);
            double yv = std::stod(tokens[col_y]);
            x.push_back(xv);
            y.push_back(yv);
        } catch (...) {
            ++row;
            continue;
        }
        ++row;
    }
    file.close();

    if (x.size() < 2) {
        std::cerr << "Недостаточно данных.\n";
        return 1;
    }

    size_t n = x.size();
    double intercept, slope, cov00, cov01, cov11, chi_squared;
    gsl_fit_linear(x.data(), 1, y.data(), 1, n,
                   &intercept, &slope, &cov00, &cov01, &cov11, &chi_squared);
    double r_value = gsl_stats_correlation(x.data(), 1, y.data(), 1, n);

    // Вывод в консоль
    std::cout << "#### Линейная аппроксимация ####\n";
    std::cout << "Угловой коэффициент (m): " << slope << "\n";
    std::cout << "Пересечение (q): " << intercept << "\n";
    std::cout << "Коэффициент корреляции: " << r_value << std::endl;

    // Сохранение результатов в текстовый файл
    std::ofstream res_file("fit_results.txt");
    res_file << "Угловой коэффициент (m): " << slope << "\n";
    res_file << "Пересечение (q): " << intercept << "\n";
    res_file << "Коэффициент корреляции: " << r_value << "\n";
    res_file.close();

    auto [min_it, max_it] = std::minmax_element(x.begin(), x.end());
    double x_min = *min_it - 1.0;
    double x_max = *max_it + 1.0;
    double step = (x_max - x_min) / (N_points - 1);

    std::vector<double> line_x, line_y;
    for (unsigned int i = 0; i < N_points; ++i) {
        double cx = x_min + step * i;
        line_x.push_back(cx);
        line_y.push_back(intercept + slope * cx);
    }

    // График через sciplot
    sciplot::Plot2D plot;
    plot.xlabel(x_label);
    plot.ylabel(y_label);
    plot.legend().atOutsideBottom();
    plot.drawPoints(x, y).label("Data").pointType(7);
    plot.drawCurve(line_x, line_y).label("Fit").lineWidth(2);

    plot.save("fit_output.png");
    plot.show();

    std::cout << "Результаты сохранены в fit_results.txt\n";
    std::cout << "График сохранён в fit_output.png\n";

    return 0;
}