#include "functions.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

vector<string> split(const string &s, char delim) {
    vector<string> result;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim))
        result.push_back(item);
    return result;
}

char detect_delimiter(const string &line) {
    if (line.find('\t') != string::npos)
        return '\t';
    return ',';
}

bool read_csv(const string &filename, int skip_rows, int col_x, int col_y,
              vector<double> &x, vector<double> &y,
              string &x_label, string &y_label) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла: " << filename << endl;
        return false;
    }

    x_label = "X";
    y_label = "Y";

    string header_line;
    if (getline(file, header_line)) {
        file.clear();
        file.seekg(0);
        char h_delim = detect_delimiter(header_line);
        vector<string> headers = split(header_line, h_delim);
        if (col_x < (int)headers.size()) x_label = headers[col_x];
        if (col_y < (int)headers.size()) y_label = headers[col_y];
    }

    string line;
    int row = 0;
    char delimiter = 0;

    while (getline(file, line)) {
        if (row < skip_rows) {
            ++row;
            continue;
        }
        if (delimiter == 0)
            delimiter = detect_delimiter(line);

        vector<string> tokens = split(line, delimiter);
        if ((int)tokens.size() <= max(col_x, col_y)) {
            ++row;
            continue;
        }
        try {
            double xv = stod(tokens[col_x]);
            double yv = stod(tokens[col_y]);
            x.push_back(xv);
            y.push_back(yv);
        } catch (...) {
        }
        ++row;
    }
    file.close();

    if (x.size() < 2) {
        cerr << "Недостаточно данных." << endl;
        return false;
    }
    return true;
}

void write_plot_files(const vector<double> &x, const vector<double> &y,
                      double intercept, double slope, int N_points,
                      const string &x_label, const string &y_label) {
    int n = x.size();

    double min_x = x[0], max_x = x[0];
    for (int i = 1; i < n; ++i) {
        if (x[i] < min_x) min_x = x[i];
        if (x[i] > max_x) max_x = x[i];
    }
    double x_min = min_x - 1.0;
    double x_max = max_x + 1.0;
    double step = (x_max - x_min) / (N_points - 1);

    ofstream points_file("data_points.dat");
    for (int i = 0; i < n; ++i)
        points_file << x[i] << "\t" << y[i] << "\n";
    points_file.close();

    ofstream line_file("fit_line.dat");
    for (int i = 0; i < N_points; ++i) {
        double cx = x_min + step * i;
        double cy = intercept + slope * cx;
        line_file << cx << "\t" << cy << "\n";
    }
    line_file.close();

    ofstream gp("plot.gp");
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
    system("gnuplot -p plot.gp"); // Gnuplot вызывается для построения графика
}