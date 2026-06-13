#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <vector>

std::vector<std::string> split(const std::string &s, char delim);
char detect_delimiter(const std::string &line);
bool read_csv(const std::string &filename, int skip_rows, int col_x, int col_y,
              std::vector<double> &x, std::vector<double> &y,
              std::string &x_label, std::string &y_label);
void write_plot_files(const std::vector<double> &x, const std::vector<double> &y,
                      double intercept, double slope, int N_points,
                      const std::string &x_label, const std::string &y_label);
void show_plot();

#endif