#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <stdexcept>
#include <algorithm>

extern "C" {
#include <gsl/gsl_fit.h>
#include <gsl/gsl_statistics_double.h>
}

#include "functions.h"

int main(int argc, char* argv[]) {
    try {
        if (argc != 6) {
            throw std::invalid_argument(std::string("Использование: ") + argv[0] +
                " <csv-файл> <пропуск строк> <столбец X> <столбец Y> <точек на прямой>");
        }

        std::string filename = argv[1];
        int skip_rows = std::stoi(argv[2]);
        int col_x = std::stoi(argv[3]);
        int col_y = std::stoi(argv[4]);
        int N_points = std::stoi(argv[5]);

        validate_inputs(skip_rows, col_x, col_y, N_points);

        std::vector<double> x, y;
        std::string x_label, y_label;
        read_csv(filename, skip_rows, col_x, col_y, x, y, x_label, y_label);

        print_data_statistics(x, y);

        double intercept, slope, r;
        compute_linear_fit(x, y, intercept, slope, r);

        std::cout << "#### Линейная аппроксимация ####\n";
        std::cout << "Угловой коэффициент (m): " << slope << "\n";
        std::cout << "Пересечение (q): " << intercept << "\n";
        std::cout << "Коэффициент корреляции: " << r << std::endl;

        write_plot_files(x, y, intercept, slope, N_points, x_label, y_label);
        show_plot();

        save_report("fit_report.txt", intercept, slope, r);
        std::cout << "График сохранён в fit_output.png\n";
        std::cout << "Текстовый отчет сохранён в fit_report.txt\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}