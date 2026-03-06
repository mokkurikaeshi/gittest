#include <iostream>
#include <cmath>
#include <vector>
#include <array>

// 3次元ベクトル
struct Vec3 {
    double x, y, z;

    Vec3 operator-(const Vec3& v) const { return {x - v.x, y - v.y, z - v.z}; }
    Vec3 operator+(const Vec3& v) const { return {x + v.x, y + v.y, z + v.z}; }
    Vec3 operator*(double s) const { return {x * s, y * s, z * s}; }

    double norm() const { return std::sqrt(x * x + y * y + z * z); }

    Vec3 normalized() const {
        double n = norm();
        if (n < 1e-12) return {0, 0, 0};
        return {x / n, y / n, z / n};
    }
};

// 外積
Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// 内積
double dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// 接触点情報
struct ContactPoint {
    Vec3 position;  // 接触位置（ワールド座標）
    Vec3 normal;    // 接触面の法線ベクトル（単位ベクトル、表面から外向き）
};

// エンドエフェクタの表面形状（三角メッシュ）
struct Triangle {
    Vec3 v0, v1, v2;

    // 三角形の面法線を計算（右手系、頂点の順序で向きが決まる）
    Vec3 face_normal() const {
        Vec3 e1 = v1 - v0;
        Vec3 e2 = v2 - v0;
        return cross(e1, e2).normalized();
    }
};

// 点と三角形の最近接判定（点が三角形内にあるか）
bool point_in_triangle(const Vec3& p, const Triangle& tri, double tol = 1e-6) {
    Vec3 v0 = tri.v1 - tri.v0;
    Vec3 v1 = tri.v2 - tri.v0;
    Vec3 v2 = p - tri.v0;

    double d00 = dot(v0, v0);
    double d01 = dot(v0, v1);
    double d11 = dot(v1, v1);
    double d20 = dot(v2, v0);
    double d21 = dot(v2, v1);

    double denom = d00 * d11 - d01 * d01;
    if (std::abs(denom) < 1e-12) return false;

    double u = (d11 * d20 - d01 * d21) / denom;
    double v = (d00 * d21 - d01 * d20) / denom;

    return (u >= -tol) && (v >= -tol) && (u + v <= 1.0 + tol);
}

// エンドエフェクタの表面メッシュから接触点の法線を計算
// surface: エンドエフェクタ表面を構成する三角メッシュ
// contact_pos: 接触が検出された位置
// 戻り値: 接触点の法線ベクトル（表面外向き）
ContactPoint compute_contact_normal(const std::vector<Triangle>& surface,
                                    const Vec3& contact_pos) {
    double min_dist = 1e18;
    Vec3 best_normal = {0, 0, 1};

    for (const auto& tri : surface) {
        // 接触点を三角形の平面に射影
        Vec3 n = tri.face_normal();
        Vec3 diff = contact_pos - tri.v0;
        double dist_to_plane = std::abs(dot(diff, n));

        // 射影点が三角形内にあるか確認
        Vec3 projected = contact_pos - n * dot(diff, n);
        if (point_in_triangle(projected, tri) && dist_to_plane < min_dist) {
            min_dist = dist_to_plane;
            best_normal = n;
        }
    }

    return {contact_pos, best_normal};
}

void print_vec3(const char* label, const Vec3& v) {
    std::printf("  %s: [%+.6f, %+.6f, %+.6f]\n", label, v.x, v.y, v.z);
}

int main() {
    // ====================================================
    // 例: 平行グリッパのエンドエフェクタ表面を定義
    // 左指の内側面（YZ平面に平行、法線が+X方向）
    // ====================================================
    std::vector<Triangle> left_finger_surface = {
        {{0.0, -0.01, 0.0}, {0.0, 0.01, 0.0}, {0.0, 0.01, 0.03}},
        {{0.0, -0.01, 0.0}, {0.0, 0.01, 0.03}, {0.0, -0.01, 0.03}},
    };

    // 右指の内側面（法線が-X方向）
    std::vector<Triangle> right_finger_surface = {
        {{0.05, 0.01, 0.0}, {0.05, -0.01, 0.0}, {0.05, -0.01, 0.03}},
        {{0.05, 0.01, 0.0}, {0.05, -0.01, 0.03}, {0.05, 0.01, 0.03}},
    };

    // 指先面（法線が-Z方向）
    std::vector<Triangle> fingertip_surface = {
        {{0.0, -0.01, 0.0}, {0.05, -0.01, 0.0}, {0.05, 0.01, 0.0}},
        {{0.0, -0.01, 0.0}, {0.05, 0.01, 0.0}, {0.0, 0.01, 0.0}},
    };

    // ====================================================
    // テスト: 各面上の接触点に対して法線ベクトルを計算
    // ====================================================
    struct TestCase {
        const char* description;
        std::vector<Triangle>* surface;
        Vec3 contact_pos;
    };

    std::vector<TestCase> tests = {
        {"左指内側面での接触",   &left_finger_surface,  {0.0,  0.0, 0.015}},
        {"右指内側面での接触",   &right_finger_surface, {0.05, 0.0, 0.015}},
        {"指先面での接触",       &fingertip_surface,    {0.025, 0.0, 0.0}},
    };

    std::cout << "============================================\n";
    std::cout << " エンドエフェクタ接触点 法線ベクトル出力\n";
    std::cout << "============================================\n\n";

    for (const auto& t : tests) {
        ContactPoint cp = compute_contact_normal(*t.surface, t.contact_pos);
        std::printf("[%s]\n", t.description);
        print_vec3("接触位置 (m)", cp.position);
        print_vec3("法線ベクトル", cp.normal);
        std::printf("  |n| = %.6f (単位ベクトル検証)\n\n", cp.normal.norm());
    }

    // ====================================================
    // 全表面を統合して任意の接触点の法線を求める例
    // ====================================================
    std::cout << "--------------------------------------------\n";
    std::cout << " 統合メッシュでの法線計算\n";
    std::cout << "--------------------------------------------\n\n";

    std::vector<Triangle> all_surfaces;
    all_surfaces.insert(all_surfaces.end(), left_finger_surface.begin(), left_finger_surface.end());
    all_surfaces.insert(all_surfaces.end(), right_finger_surface.begin(), right_finger_surface.end());
    all_surfaces.insert(all_surfaces.end(), fingertip_surface.begin(), fingertip_surface.end());

    Vec3 query_point = {0.0, 0.005, 0.02};
    ContactPoint result = compute_contact_normal(all_surfaces, query_point);

    std::printf("[任意の接触点での法線]\n");
    print_vec3("接触位置 (m)", result.position);
    print_vec3("法線ベクトル", result.normal);
    std::printf("  |n| = %.6f\n", result.normal.norm());

    return 0;
}
