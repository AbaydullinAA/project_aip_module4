#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <vector>

/**
 * @brief Разбивает строку на подстроки по заданному разделителю.
 *
 * Функция создаёт поток из переданной строки и последовательно извлекает
 * подстроки, разделённые символом @p delim, пока поток не закончится.
 *
 * @param s Исходная строка, которую нужно разбить.
 * @param delim Символ-разделитель.
 * @return std::vector<std::string> Вектор, содержащий полученные подстроки.
 */
std::vector<std::string> split(const std::string &s, char delim);

/**
 * @brief Определяет символ-разделитель в строке.
 *
 * Проверяет, содержит ли строка символ табуляции. Если содержит, возвращает
 * `'\t'`, иначе возвращает `','`. Используется для автоматического определения
 * формата CSV (табуляция или запятая).
 *
 * @param line Строка для анализа.
 * @return char `'\t'`, если в строке найден символ табуляции, иначе `','`.
 */
char detect_delimiter(const std::string &line);

/**
 * @brief Читает данные из CSV-файла и заполняет переданные контейнеры.
 *
 * Открывает файл с именем @p filename, пропускает первые @p skip_rows строк,
 * затем извлекает координаты точек из столбцов с индексами @p col_x и @p col_y
 * (индексация с 0). Прочитанные значения добавляются в векторы @p x и @p y.
 * Названия осей извлекаются из первой строки (заголовка) и записываются
 * в @p x_label и @p y_label.
 *
 * @param filename Имя CSV-файла.
 * @param skip_rows Количество строк, которые нужно пропустить в начале файла.
 * @param col_x Индекс столбца для оси X (начиная с 0).
 * @param col_y Индекс столбца для оси Y (начиная с 0).
 * @param[out] x Вектор, в который будут записаны X-координаты точек.
 * @param[out] y Вектор, в который будут записаны Y-координаты точек.
 * @param[out] x_label Строка, в которую будет записано название оси X.
 * @param[out] y_label Строка, в которую будет записано название оси Y.
 *
 * @throws std::runtime_error если файл не удалось открыть или в нём меньше двух точек.
 */
void read_csv(const std::string &filename, int skip_rows, int col_x, int col_y,
              std::vector<double> &x, std::vector<double> &y,
              std::string &x_label, std::string &y_label);

/**
 * @brief Записывает файлы данных и управляющий скрипт для Gnuplot.
 *
 * По переданным точкам и коэффициентам прямой создаются:
 * - @c data_points.dat с исходными точками (табуляция в качестве разделителя);
 * - @c fit_line.dat с @p N_points точками прямой, равномерно распределёнными
 *   по оси X;
 * - @c plot.gp — скрипт Gnuplot, который строит оба набора данных.
 *
 * @param x Вектор X-координат исходных точек.
 * @param y Вектор Y-координат исходных точек.
 * @param intercept Свободный член линейной аппроксимации (пересечение).
 * @param slope Угловой коэффициент линейной аппроксимации.
 * @param N_points Количество точек, которое будет сгенерировано для линии.
 * @param x_label Подпись для оси X.
 * @param y_label Подпись для оси Y.
 */

/**
 * @brief Вычисляет линейную регрессию и коэффициент корреляции.
 *
 * Использует GSL для расчёта углового коэффициента (slope), свободного члена
 * (intercept) и коэффициента корреляции Пирсона (r) по переданным векторам x и y.
 *
 * @param x Вектор X-координат точек.
 * @param y Вектор Y-координат точек.
 * @param[out] intercept Свободный член линейной аппроксимации.
 * @param[out] slope Угловой коэффициент линейной аппроксимации.
 * @param[out] r Коэффициент корреляции Пирсона.
 * @throws std::runtime_error если количество точек меньше 2.
 */
void compute_linear_fit(const std::vector<double> &x, const std::vector<double> &y,
                        double &intercept, double &slope, double &r);

void write_plot_files(const std::vector<double> &x, const std::vector<double> &y,
                      double intercept, double slope, int N_points,
                      const std::string &x_label, const std::string &y_label);

/**
 * @brief Запускает Gnuplot для отрисовки графика.
 *
 * Выполняет команду `gnuplot -p plot.gp`, которая открывает окно с графиком
 * и сохраняет его в файл `fit_output.png`.
 */
void show_plot();

#endif