#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

// Используем GSL для расчёта линейной регрессии
extern "C" {
#include <gsl/gsl_fit.h>
#include <gsl/gsl_statistics_double.h>
}

#include "functions.h"

int main(int argc, char* argv[]) {
    if (argc != 6) {
        std::cerr << "Использование: " << argv[0]
                  << " <csv-файл> <пропуск строк> <столбец X> <столбец Y> <точек на прямой>\n";
        return 1;
    }

    std::string filename = argv[1];
    int skip_rows = std::stoi(argv[2]);
    int col_x = std::stoi(argv[3]);
    int col_y = std::stoi(argv[4]);
    int N_points = std::stoi(argv[5]);

    std::vector<double> x, y;
    std::string x_label, y_label;
    if (!read_csv(filename, skip_rows, col_x, col_y, x, y, x_label, y_label)) {
        return 1;
    }

    int n = x.size();
    double intercept, slope, cov00, cov01, cov11, chi_sq;
    gsl_fit_linear(&x[0], 1, &y[0], 1, n,
                   &intercept, &slope,
                   &cov00, &cov01, &cov11, &chi_sq);
    double r = gsl_stats_correlation(&x[0], 1, &y[0], 1, n);

    std::cout << "#### Линейная аппроксимация ####\n";
    std::cout << "Угловой коэффициент (m): " << slope << "\n";
    std::cout << "Пересечение (q): " << intercept << "\n";
    std::cout << "Коэффициент корреляции: " << r << std::endl;

    write_plot_files(x, y, intercept, slope, N_points, x_label, y_label);
    show_plot();

    std::cout << "График сохранён в fit_output.png\n";
    return 0;
}