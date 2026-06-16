#include "functions.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
#include <algorithm>

extern "C" {
#include <gsl/gsl_fit.h>
#include <gsl/gsl_statistics_double.h>
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
        result.push_back(item);
    return result;
}

char detect_delimiter(const std::string &line) {
    if (line.find('\t') != std::string::npos)
        return '\t';
    return ',';
}

void validate_inputs(int skip_rows, int col_x, int col_y, int N_points) {
    if (skip_rows < 0)
        throw std::invalid_argument("Количество пропускаемых строк не может быть отрицательным.");
    if (col_x < 0)
        throw std::invalid_argument("Индекс столбца X не может быть отрицательным.");
    if (col_y < 0)
        throw std::invalid_argument("Индекс столбца Y не может быть отрицательным.");
    if (col_x == col_y)
        throw std::invalid_argument("Столбцы X и Y не должны совпадать.");
    if (N_points < 2)
        throw std::invalid_argument("Количество точек для линии должно быть не менее 2.");
    if (skip_rows > 10000)
        throw std::invalid_argument("Количество пропускаемых строк подозрительно велико.");
}

void read_csv(const std::string &filename, int skip_rows, int col_x, int col_y,
              std::vector<double> &x, std::vector<double> &y,
              std::string &x_label, std::string &y_label) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Ошибка открытия файла: " + filename);
    }

    if (file.peek() == std::ifstream::traits_type::eof()) {
        throw std::runtime_error("Файл пуст: " + filename);
    }

    x_label = "X";
    y_label = "Y";

    std::string header_line;
    if (std::getline(file, header_line)) {
        file.clear();
        file.seekg(0);
        char h_delim = detect_delimiter(header_line);
        std::vector<std::string> headers = split(header_line, h_delim);
        if (col_x < (int)headers.size()) x_label = headers[col_x];
        if (col_y < (int)headers.size()) y_label = headers[col_y];
    }

    std::string line;
    int row = 0;
    char delimiter = 0;

    while (std::getline(file, line)) {
        if (row < skip_rows) {
            ++row;
            continue;
        }
        if (delimiter == 0)
            delimiter = detect_delimiter(line);

        std::vector<std::string> tokens = split(line, delimiter);
        if ((int)tokens.size() <= std::max(col_x, col_y)) {
            ++row;
            continue;
        }
        try {
            double xv = std::stod(tokens[col_x]);
            double yv = std::stod(tokens[col_y]);
            x.push_back(xv);
            y.push_back(yv);
        } catch (...) {
        }
        ++row;
    }
    file.close();

    if (x.size() < 2) {
        throw std::runtime_error("Недостаточно данных: нужно хотя бы 2 точки.");
    }
}

void print_data_statistics(const std::vector<double> &x, const std::vector<double> &y) {
    if (x.empty() || y.empty()) return;
    double sum_x = 0.0, sum_y = 0.0;
    double min_x = x[0], max_x = x[0];
    double min_y = y[0], max_y = y[0];
    for (size_t i = 0; i < x.size(); ++i) {
        sum_x += x[i];
        sum_y += y[i];
        if (x[i] < min_x) min_x = x[i];
        if (x[i] > max_x) max_x = x[i];
        if (y[i] < min_y) min_y = y[i];
        if (y[i] > max_y) max_y = y[i];
    }
    double mean_x = sum_x / x.size();
    double mean_y = sum_y / y.size();
    double var_x = 0.0, var_y = 0.0;
    for (size_t i = 0; i < x.size(); ++i) {
        var_x += (x[i] - mean_x) * (x[i] - mean_x);
        var_y += (y[i] - mean_y) * (y[i] - mean_y);
    }
    var_x /= x.size();
    var_y /= y.size();
    std::cout << "#### Статистика входных данных ####\n";
    std::cout << "Среднее X: " << mean_x << ", Дисперсия X: " << var_x << "\n";
    std::cout << "Среднее Y: " << mean_y << ", Дисперсия Y: " << var_y << "\n";
    std::cout << "Диапазон X: [" << min_x << ", " << max_x << "]\n";
    std::cout << "Диапазон Y: [" << min_y << ", " << max_y << "]\n\n";
}

void save_report(const std::string &filename, double intercept, double slope, double r) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("Ошибка создания файла отчета: " + filename);
    }
    out << "#### Отчет по линейной аппроксимации ####\n";
    out << "Угловой коэффициент (slope): " << slope << "\n";
    out << "Пересечение (intercept): " << intercept << "\n";
    out << "Коэффициент корреляции (r): " << r << "\n";
    out << "Коэффициент детерминации (R^2): " << r * r << "\n";
    out.close();
}

void compute_linear_fit(const std::vector<double> &x, const std::vector<double> &y,
                        double &intercept, double &slope, double &r) {
    size_t n = x.size();
    if (n < 2) {
        throw std::runtime_error("Для линейной регрессии нужно минимум 2 точки.");
    }
    double cov00, cov01, cov11, chi_sq;
    gsl_fit_linear(x.data(), 1, y.data(), 1, n,
                   &intercept, &slope,
                   &cov00, &cov01, &cov11, &chi_sq);
    r = gsl_stats_correlation(x.data(), 1, y.data(), 1, n);
}

void write_plot_files(const std::vector<double> &x, const std::vector<double> &y,
                      double intercept, double slope, int N_points,
                      const std::string &x_label, const std::string &y_label) {
    int n = x.size();
    if (N_points < 2) {
        throw std::runtime_error("Количество точек для линии должно быть не менее 2.");
    }

    double min_x = x[0], max_x = x[0];
    for (int i = 1; i < n; ++i) {
        if (x[i] < min_x) min_x = x[i];
        if (x[i] > max_x) max_x = x[i];
    }
    double x_min = min_x - 1.0;
    double x_max = max_x + 1.0;
    double step = (x_max - x_min) / (N_points - 1);

    std::ofstream points_file("data_points.dat");
    for (int i = 0; i < n; ++i)
        points_file << x[i] << "\t" << y[i] << "\n";
    points_file.close();

    std::ofstream line_file("fit_line.dat");
    for (int i = 0; i < N_points; ++i) {
        double cx = x_min + step * i;
        double cy = intercept + slope * cx;
        line_file << cx << "\t" << cy << "\n";
    }
    line_file.close();

    std::ofstream gp("plot.gp");
    gp << "set terminal png size 800,600\n";
    gp << "set output 'fit_output.png'\n";
    gp << "set xlabel '" << x_label << "'\n";
    gp << "set ylabel '" << y_label << "'\n";
    gp << "set title 'Linear Fit'\n";
    gp << "plot 'data_points.dat' using 1:2 with points pt 7 title 'Data', "
       << "'fit_line.dat' using 1:2 with lines lw 2 title 'Fit'\n";
    gp.close();
}

void show_plot() {
    system("gnuplot -p plot.gp");
}