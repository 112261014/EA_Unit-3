#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

using namespace std;

// 地圖大小
const int MAP_SIZE = 46;
const int NUM_TERRAINS = 5;
const int POPULATION_SIZE = 6;  // 族群大小
const int TOURNAMENT_SIZE = 3;   // 比賽個體數量
const int GENERATIONS = 4000;    // 迭代次數
int MUTATION_POINTS = 10;        // 突變點數

// 全域隨機生成器
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis(0, NUM_TERRAINS - 1);
uniform_int_distribution<> tournament_dis(0, POPULATION_SIZE - 1);
uniform_int_distribution<> mutation_point_dis(0, MAP_SIZE* MAP_SIZE - 1);

// 隨機生成一個地圖，將其轉換為一維陣列
void generate_random_map(vector<int>& map) {
    for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
        map[i] = dis(gen);
    }
}

// 計算一個地形類型的變異數
double calculate_variance(const vector<int>& map, int terrain_type) {
    vector<pair<int, int>> positions;

    // 找到所有符合 terrain_type 的坐標
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            if (map[i * MAP_SIZE + j] == terrain_type) {
                positions.push_back({ i, j });
            }
        }
    }

    if (positions.empty()) {
        return 0.0;
    }

    // 計算平均位置
    double mean_x = 0, mean_y = 0;
    for (const auto& pos : positions) {
        mean_x += pos.first;
        mean_y += pos.second;
    }
    mean_x /= positions.size();
    mean_y /= positions.size();

    // 計算距離並求變異數
    double variance = 0;
    for (const auto& pos : positions) {
        double dx = pos.first - mean_x;
        double dy = pos.second - mean_y;
        variance += dx * dx + dy * dy;
    }
    variance /= positions.size();

    return variance;
}

// 計算地圖的適應度
double fitness(const vector<int>& map) {
    double total_variance = 0;

    // 計算每種地形類型的變異數
    for (int terrain_type = 0; terrain_type < NUM_TERRAINS; ++terrain_type) {
        total_variance += calculate_variance(map, terrain_type);
    }

    return total_variance; // 適應度越小越好
}

// 隨機交叉
void random_crossover(const vector<int>& parent1, const vector<int>& parent2, vector<int>& child1, vector<int>& child2) {
    for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
        // 隨機從父代1或父代2選擇基因
        if (uniform_int_distribution<>(0, 1)(gen) == 0) {
            child1[i] = parent1[i];
            child2[i] = parent2[i];
        }
        else {
            child1[i] = parent2[i];
            child2[i] = parent1[i];
        }
    }
}

// 隨機突變
void mutate(vector<int>& individual) {
    for (int i = 0; i < MUTATION_POINTS; ++i) {
        int mutation_index = mutation_point_dis(gen); // 隨機選擇突變位置
        individual[mutation_index] = dis(gen);        // 隨機改變地形
    }
}

// 生成族群
void generate_population(vector<vector<int>>& population) {
    for (int i = 0; i < POPULATION_SIZE; ++i) {
        generate_random_map(population[i]);
    }
}

// Tournament Selection
int tournament_selection(const vector<vector<int>>& population) {
    double best_fitness = 1e9;  // 越小越好
    int best_index = -1;

    for (int i = 0; i < TOURNAMENT_SIZE; ++i) {
        int idx = tournament_dis(gen);  // 隨機選擇參賽的個體
        double fit = fitness(population[idx]);

        if (fit < best_fitness) { // 選擇適應度最小的個體
            best_fitness = fit;
            best_index = idx;
        }
    }

    return best_index;  // 返回最適應的個體索引
}

// 印出地圖 (一維轉二維)
void print_map(const vector<int>& map) {
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            cout << map[i * MAP_SIZE + j];
        }
        cout << "\n";
    }
    cout << "\n";
}

int main() {
    for (int rnd = 0; rnd < 10; rnd++) {
        // 初始化族群
        vector<vector<int>> population(POPULATION_SIZE, vector<int>(MAP_SIZE * MAP_SIZE));
        generate_population(population);

        for (int generation = 0; generation < GENERATIONS; ++generation) {
            vector<vector<int>> new_population(POPULATION_SIZE, vector<int>(MAP_SIZE * MAP_SIZE));
            if (generation > 500) MUTATION_POINTS = 1;

            // 每次迭代產生新的子代
            for (int i = 0; i < POPULATION_SIZE; i += 2) {
                // 選擇兩個父代
                int parent1_idx = tournament_selection(population);
                int parent2_idx = tournament_selection(population);

                // 產生兩個子代
                vector<int> child1(MAP_SIZE * MAP_SIZE), child2(MAP_SIZE * MAP_SIZE);
                random_crossover(population[parent1_idx], population[parent2_idx], child1, child2);

                // 突變
                mutate(child1);
                mutate(child2);

                // 放入新族群
                new_population[i] = child1;
                new_population[i + 1] = child2;
            }

            // 用子代更新族群
            population = new_population;

            // 計算並輸出目前族群的平均適應度
            double total_fitness = 0;
            for (const auto& individual : population) {
                total_fitness += fitness(individual);
            }
            cout << "Generation " << generation + 1 << ": Average Fitness = " << total_fitness / POPULATION_SIZE << endl;
        }

        // 印出最終族群中的一個個體
        cout << "\nFinal individual map:\n";
        print_map(population[0]);
        cin.ignore();
    }
    return 0;
}
