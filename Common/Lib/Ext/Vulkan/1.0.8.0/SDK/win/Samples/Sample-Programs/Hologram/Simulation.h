/*
 * Copyright (C) 2016 Google, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef SIMULATION_H
#define SIMULATION_H

#include <memory>
#include <random>
#include <vector>

#include <glm/glm.hpp>

#include "Meshes.h"

class Animation {
public:
    Animation(unsigned rng_seed, float scale);

    glm::mat4 transformation(float t);
    float transparency();

private:
    struct Data {
        glm::vec3 axis;
        float speed;
        float scale;

        glm::mat4 matrix;

        float alpha;
        float alpha_inc;
    };

    std::mt19937 rng_;
    std::uniform_real_distribution<float> dir_;
    std::uniform_real_distribution<float> speed_;

    Data current_;
};

class Curve;

class Path {
public:
    Path(unsigned rng_seed);

    glm::vec3 position(float t);

private:
    struct Subpath {
        glm::vec3 origin;
        float start;
        float end;
        float now;

        std::shared_ptr<Curve> curve;
    };

    void generate_subpath();

    std::mt19937 rng_;
    std::uniform_int_distribution<> type_;
    std::uniform_real_distribution<float> duration_;

    Subpath current_;
};

class Simulation {
public:
    Simulation(int object_count);

    struct Object {
        Meshes::Type mesh;
        glm::vec3 light_pos;
        glm::vec3 light_color;

        Animation animation;
        Path path;

        uint32_t frame_data_offset;

        glm::mat4 model;
        float alpha;
    };

    const std::vector<Object> &objects() const { return objects_; }

    unsigned int rng_seed() { return random_dev_(); }

    void set_frame_data_size(uint32_t size);
    void update(float time, int begin, int end);

private:
    std::random_device random_dev_;
    std::vector<Object> objects_;
};

#endif // SIMULATION_H
