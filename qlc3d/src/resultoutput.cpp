//
// Created by eero on 09/04/2021.
//
#include <resultoutput.h>
#include <fstream>
#include <stdexcept>
#include <lc-representation.h>
void ResultOutput::writeCsvUnstructured(const double *p,
                                        const SolutionVector &v,
                                        const SolutionVector &q,
                                        const std::string &fileName) {
    std::fstream fs(fileName, fs.out);
    if (!fs.is_open()) {
        throw std::runtime_error("could not open output file:" + fileName);
    }

    idx stride = v.getnDoF();
    fs << "x, y, z, v, nx, ny, nz" << std::endl;
    for (int i = 0; i < v.getnDoF(); i++) {
        // TODO: handle dielectric regions where no LC material exists

        auto n = qlc3d::TTensor {
            q.getValue(i, 0),
            q.getValue(i, 1),
            q.getValue(i, 2),
            q.getValue(i, 3),
            q.getValue(i, 4)
        }.toDirector();
        fs << p[3 * i] << "," << p[3 * i + 1] << "," << p[3 * i + 2] << "," <<
           v.getValue(i) << "," << n.nx() << "," << n.ny() << "," << n.nz() << std::endl;
    }

    fs.close();
}
