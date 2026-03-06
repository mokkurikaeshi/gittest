import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import numpy as np

fig = plt.figure(figsize=(14, 6))

# ============================================================
# グリッパ形状の定義 (単位: m → mm表示)
# ============================================================
scale = 1000  # m → mm

# 左指 (x=0 面)
left_finger = [
    [[0, -10, 0], [0, 10, 0], [0, 10, 30], [0, -10, 30]],
]
# 右指 (x=50 面)
right_finger = [
    [[50, -10, 0], [50, 10, 0], [50, 10, 30], [50, -10, 30]],
]
# 指先面 (z=0 面)
fingertip = [
    [[0, -10, 0], [50, -10, 0], [50, 10, 0], [0, 10, 0]],
]

# 接触点と法線ベクトル
contacts = [
    {"pos": [0, 0, 15],   "normal": [1, 0, 0],   "label": "Left Inner",  "color": "#e74c3c"},
    {"pos": [50, 0, 15],  "normal": [-1, 0, 0],  "label": "Right Inner", "color": "#2ecc71"},
    {"pos": [25, 0, 0],   "normal": [0, 0, 1],   "label": "Fingertip",   "color": "#3498db"},
    {"pos": [0, 5, 20],   "normal": [1, 0, 0],   "label": "Arbitrary",   "color": "#f39c12"},
]

arrow_len = 15  # 法線矢印の長さ (mm)

# ============================================================
# ビュー1: 3Dパース
# ============================================================
ax1 = fig.add_subplot(121, projection='3d')
ax1.set_title("3D View: End-Effector Contact Normals", fontsize=11, fontweight='bold')

for faces, color, alpha in [
    (left_finger, '#e74c3c', 0.25),
    (right_finger, '#2ecc71', 0.25),
    (fingertip, '#3498db', 0.20),
]:
    poly = Poly3DCollection(faces, alpha=alpha, facecolor=color, edgecolor=color, linewidth=1.5)
    ax1.add_collection3d(poly)

for c in contacts:
    p = np.array(c["pos"])
    n = np.array(c["normal"]) * arrow_len
    ax1.scatter(*p, color=c["color"], s=60, zorder=5, depthshade=False)
    ax1.quiver(*p, *n, color=c["color"], arrow_length_ratio=0.2, linewidth=2.5)
    ax1.text(p[0] + n[0]*1.3, p[1] + n[1]*1.3, p[2] + n[2]*1.3,
             c["label"], fontsize=8, color=c["color"])

ax1.set_xlabel("X (mm)")
ax1.set_ylabel("Y (mm)")
ax1.set_zlabel("Z (mm)")
ax1.set_xlim(-15, 70)
ax1.set_ylim(-20, 20)
ax1.set_zlim(-10, 40)
ax1.view_init(elev=25, azim=-50)

# ============================================================
# ビュー2: 上面図 (XZ平面, Y=0)
# ============================================================
ax2 = fig.add_subplot(122)
ax2.set_title("Top View (XZ Plane): Contact Normals", fontsize=11, fontweight='bold')
ax2.set_aspect('equal')

# グリッパ断面を描画
left_x = [0, 0]
left_z = [0, 30]
right_x = [50, 50]
right_z = [0, 30]
bottom_x = [0, 50]
bottom_z = [0, 0]

ax2.plot(left_x, left_z, color='#e74c3c', linewidth=4, label='Left Finger')
ax2.plot(right_x, right_z, color='#2ecc71', linewidth=4, label='Right Finger')
ax2.plot(bottom_x, bottom_z, color='#3498db', linewidth=4, label='Fingertip')

# 把持対象（イメージ）
rect = plt.Rectangle((10, 2), 30, 26, fill=True, facecolor='#ecf0f1',
                      edgecolor='#95a5a6', linewidth=1.5, linestyle='--', label='Object')
ax2.add_patch(rect)

for c in contacts:
    p = np.array(c["pos"])
    n = np.array(c["normal"]) * arrow_len
    ax2.plot(p[0], p[2], 'o', color=c["color"], markersize=8, zorder=5)
    ax2.annotate('', xy=(p[0] + n[0], p[2] + n[2]), xytext=(p[0], p[2]),
                 arrowprops=dict(arrowstyle='->', color=c["color"], lw=2.5))
    offset_x = 3 if n[0] >= 0 else -3
    offset_z = 3 if n[2] >= 0 else -3
    ax2.annotate(f'{c["label"]}\nn={list(map(int, c["normal"]))}',
                 xy=(p[0] + n[0]*0.7, p[2] + n[2]*0.7),
                 fontsize=7, color=c["color"],
                 ha='center', va='bottom')

ax2.set_xlabel("X (mm)")
ax2.set_ylabel("Z (mm)")
ax2.set_xlim(-20, 75)
ax2.set_ylim(-15, 45)
ax2.legend(loc='upper left', fontsize=8)
ax2.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig("contact_normals.png", dpi=150, bbox_inches='tight')
print("保存完了: contact_normals.png")
