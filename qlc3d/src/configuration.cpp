//
// Created by eero on 02/04/2021.
//
#include <filesystem>
#include <settings-reader.h>
#include <cassert>
#include "configuration.h"

Configuration::Configuration() :
        #if Windows
            settingsFileName_(".\\meshes\\test.txt"), // default for backwards compatibility
        #elif Linux
            settingsFileName_("./meshes/test.txt"), // default for backwards compatibility
        #endif
        currentDirectory_(std::filesystem::current_path().string().c_str()),
        simu_(nullptr)
        {}

void Configuration::readSettings() {
    SettingsReader reader(settingsFileName());
    simu_ = reader.simu();
    lc_ = reader.lc();
    refinement_ = reader.refinement();
}

std::shared_ptr<Simu> Configuration::simu() const {
    assert(simu_ != nullptr);
    return simu_;
}

std::shared_ptr<LC> Configuration::lc() const {
    assert(lc_ != nullptr);
    return lc_;
}

std::shared_ptr<MeshRefinement> Configuration::refinement() const {
    assert(refinement_ != nullptr);
    return refinement_;
}