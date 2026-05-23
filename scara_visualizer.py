#!/usr/bin/env python3
"""
SCARA Robot Visualizer
Interactive visualization showing how SCARA robots work
"""

import math
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.patches import FancyArrowPatch, Circle, Rectangle, FancyBboxPatch
import numpy as np

def draw_scara_robot_diagram():
    """Create educational diagram of SCARA robot components"""
    
    fig, axes = plt.subplots(2, 2, figsize=(16, 14))
    fig.suptitle('🎓 SCARA Robot: Components & Kinematics Explained', fontsize=16, fontweight='bold')
    
    # ============================================================
    # Diagram 1: Basic Structure
    # ============================================================
    ax1 = axes[0, 0]
    ax1.set_xlim(-200, 200)
    ax1.set_ylim(-50, 250)
    ax1.set_aspect('equal')
    ax1.grid(True, alpha=0.3)
    ax1.set_title('1️⃣ Basic SCARA Structure', fontsize=12, fontweight='bold')
    
    # Base
    base = patches.Rectangle((-30, -20), 60, 25, linewidth=2, edgecolor='black', facecolor='gray')
    ax1.add_patch(base)
    ax1.annotate('BASE\n(Motor 1)', xy=(0, -7), ha='center', fontsize=9)
    
    # Arm 1
    ax1.annotate('', xy=(100, 0), xytext=(0, 0),
                arrowprops=dict(arrowstyle='-|>', color='blue', lw=4))
    ax1.plot([0, 100], [0, 0], 'b-', linewidth=8)
    ax1.annotate('ARM 1 (L1)\nUpper Arm', xy=(50, 10), ha='center', fontsize=9, color='blue')
    
    # Joint 1
    joint1 = Circle((0, 0), 12, facecolor='orange', edgecolor='black', linewidth=2)
    ax1.add_patch(joint1)
    ax1.annotate('θ1', xy=(15, 15), fontsize=11, fontweight='bold')
    
    # Arm 2
    theta2 = 60
    x2 = 100 + 80 * math.cos(math.radians(theta2))
    y2 = 100 + 80 * math.sin(math.radians(theta2))
    ax1.annotate('', xy=(x2, y2), xytext=(100, 0),
                arrowprops=dict(arrowstyle='-|>', color='green', lw=4))
    ax1.plot([100, x2], [0, y2], 'g-', linewidth=8)
    ax1.annotate('ARM 2 (L2)\nForearm', xy=(100 + 30, 50), fontsize=9, color='green')
    
    # Joint 2
    joint2 = Circle((100, 0), 10, facecolor='orange', edgecolor='black', linewidth=2)
    ax1.add_patch(joint2)
    ax1.annotate('θ2', xy=(110, 15), fontsize=11, fontweight='bold')
    
    # End Effector
    end_effector = Circle((x2, y2), 8, facecolor='red', edgecolor='black', linewidth=2)
    ax1.add_patch(end_effector)
    ax1.annotate('END EFFECTOR\n(Gripper)', xy=(x2, y2+20), ha='center', fontsize=9, color='red')
    
    # Labels
    ax1.annotate('L1 = 100mm\nL2 = 80mm', xy=(-150, 200), fontsize=10,
                bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
    
    # ============================================================
    # Diagram 2: Forward Kinematics
    # ============================================================
    ax2 = axes[0, 1]
    ax2.set_xlim(-50, 250)
    ax2.set_ylim(-50, 200)
    ax2.set_aspect('equal')
    ax2.grid(True, alpha=0.3)
    ax2.set_title('2️⃣ Forward Kinematics: Angles → Position', fontsize=12, fontweight='bold')
    
    # Show the calculation
    theta1 = 45
    theta2 = 60
    L1, L2 = 100, 80
    
    # Calculate positions
    theta1_rad = math.radians(theta1)
    theta2_rad = math.radians(theta2)
    
    joint1_x = L1 * math.cos(theta1_rad)
    joint1_y = L1 * math.sin(theta1_rad)
    end_x = joint1_x + L2 * math.cos(theta1_rad + theta2_rad)
    end_y = joint1_y + L2 * math.sin(theta1_rad + theta2_rad)
    
    # Draw robot
    ax2.plot(0, 0, 'ko', markersize=15)
    ax2.plot([0, joint1_x], [0, joint1_y], 'b-', linewidth=6, label=f'Arm 1 (L1={L1}mm)')
    ax2.plot([joint1_x, end_x], [joint1_y, end_y], 'g-', linewidth=6, label=f'Arm 2 (L2={L2}mm)')
    ax2.plot(end_x, end_y, 'ro', markersize=12)
    
    # Draw angle arcs
    arc1 = patches.Arc((0, 0), 40, 40, angle=0, theta1=0, theta2=theta1, color='blue', linewidth=2)
    ax2.add_patch(arc1)
    arc2 = patches.Arc((joint1_x, joint1_y), 30, 30, angle=theta1, theta1=0, theta2=theta2, color='green', linewidth=2)
    ax2.add_patch(arc2)
    
    # Labels
    ax2.annotate(f'θ1 = {theta1}°\nθ2 = {theta2}°', xy=(180, 150), fontsize=11,
                bbox=dict(boxstyle='round', facecolor='lightyellow', alpha=0.9))
    ax2.annotate(f'X = {end_x:.1f}mm\nY = {end_y:.1f}mm', xy=(180, 100), fontsize=11,
                bbox=dict(boxstyle='round', facecolor='lightgreen', alpha=0.9))
    ax2.annotate('θ1', xy=(20, 15), fontsize=12, fontweight='bold')
    ax2.annotate('θ2', xy=(joint1_x + 15, joint1_y + 10), fontsize=12, fontweight='bold')
    
    # Formula box
    formula_text = '''📐 Formulas:
X = L1×cos(θ1) + L2×cos(θ1+θ2)
Y = L1×sin(θ1) + L2×sin(θ1+θ2)

Example:
X = 100×cos(45°) + 80×cos(105°)
X = 70.7 + (-20.7) = 50mm
Y = 100×sin(45°) + 80×sin(105°)
Y = 70.7 + 77.3 = 148mm'''
    
    ax2.text(0.02, 0.02, formula_text, transform=ax2.transAxes, fontsize=9,
            verticalalignment='bottom', fontfamily='monospace',
            bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.9))
    
    ax2.legend(loc='lower right')
    
    # ============================================================
    # Diagram 3: Inverse Kinematics
    # ============================================================
    ax3 = axes[1, 0]
    ax3.set_xlim(-50, 250)
    ax3.set_ylim(-50, 200)
    ax3.set_aspect('equal')
    ax3.grid(True, alpha=0.3)
    ax3.set_title('3️⃣ Inverse Kinematics: Position → Angles (The Hard Part!)', fontsize=12, fontweight='bold')
    
    # Target point
    target_x, target_y = 120, 80
    D = math.sqrt(target_x**2 + target_y**2)
    
    # Calculate IK
    cos_theta2 = (L1**2 + L2**2 - D**2) / (2 * L1 * L2)
    theta2_calc = math.degrees(math.acos(cos_theta2))
    alpha = math.degrees(math.atan2(target_y, target_x))
    beta = math.degrees(math.atan2(L2 * math.sin(math.radians(theta2_calc)), 
                                    L1 + L2 * math.cos(math.radians(theta2_calc))))
    theta1_calc = alpha - beta
    
    # Draw workspace
    circle_outer = plt.Circle((0, 0), L1 + L2, fill=False, linestyle='--', color='red', alpha=0.5)
    circle_inner = plt.Circle((0, 0), abs(L1 - L2), fill=False, linestyle='--', color='blue', alpha=0.5)
    ax3.add_patch(circle_outer)
    ax3.add_patch(circle_inner)
    ax3.annotate('Max Reach\n(180mm)', xy=(150, 150), fontsize=9, color='red', alpha=0.7)
    ax3.annotate('Min Reach\n(20mm)', xy=(25, 25), fontsize=9, color='blue', alpha=0.7)
    
    # Draw target
    ax3.plot(target_x, target_y, 'r*', markersize=20, label=f'Target ({target_x}, {target_y})')
    
    # Draw solution
    joint1_x = L1 * math.cos(math.radians(theta1_calc))
    joint1_y = L1 * math.sin(math.radians(theta1_calc))
    end_x_calc = joint1_x + L2 * math.cos(math.radians(theta1_calc + theta2_calc))
    end_y_calc = joint1_y + L2 * math.sin(math.radians(theta1_calc + theta2_calc))
    
    ax3.plot(0, 0, 'ko', markersize=15)
    ax3.plot([0, joint1_x], [0, joint1_y], 'b-', linewidth=6)
    ax3.plot([joint1_x, end_x_calc], [joint1_y, end_y_calc], 'g-', linewidth=6)
    ax3.plot(end_x_calc, end_y_calc, 'go', markersize=12)
    
    # Distance line
    ax3.plot([0, target_x], [0, target_y], 'r--', alpha=0.5)
    ax3.annotate(f'D = {D:.1f}mm', xy=(target_x/2 - 10, target_y/2 + 10), fontsize=10, color='red')
    
    # Formula box
    ik_formula = f'''🔧 Law of Cosines:

Given: Target (X={target_x}, Y={target_y})
Distance D = √(X² + Y²) = {D:.1f}mm

θ2 = cos⁻¹((L1² + L2² - D²) / (2×L1×L2))
θ2 = cos⁻¹({cos_theta2:.3f})
θ2 = {theta2_calc:.1f}°

θ1 = atan2(Y, X) - atan2(L2×sin(θ2), L1+L2×cos(θ2))
θ1 = {alpha:.1f}° - {beta:.1f}°
θ1 = {theta1_calc:.1f}°'''
    
    ax3.text(0.02, 0.98, ik_formula, transform=ax3.transAxes, fontsize=9,
            verticalalignment='top', fontfamily='monospace',
            bbox=dict(boxstyle='round', facecolor='lightyellow', alpha=0.9))
    
    ax3.legend(loc='lower right')
    
    # ============================================================
    # Diagram 4: Pick and Place Example
    # ============================================================
    ax4 = axes[1, 1]
    ax4.set_xlim(-200, 200)
    ax4.set_ylim(-50, 250)
    ax4.set_aspect('equal')
    ax4.grid(True, alpha=0.3)
    ax4.set_title('4️⃣ Pick and Place Sequence (Real Application!)', fontsize=12, fontweight='bold')
    
    # Define positions
    pick_pos = (100, 0)
    place_pos = (-80, 80)
    safe_height = 50
    
    robot = type('Robot', (), {'L1': L1, 'L2': L2, 'theta1': 0, 'theta2': 0})()
    
    def get_angles(x, y):
        D = math.sqrt(x**2 + y**2)
        cos_t2 = (L1**2 + L2**2 - D**2) / (2 * L1 * L2)
        t2 = math.degrees(math.acos(cos_t2))
        alpha = math.degrees(math.atan2(y, x))
        beta = math.degrees(math.atan2(L2 * math.sin(math.radians(t2)), L1 + L2 * math.cos(math.radians(t2))))
        return alpha - beta, t2
    
    # Draw objects
    pick_item = patches.Rectangle((pick_pos[0]-10, pick_pos[1]-5), 20, 10, 
                                   facecolor='brown', edgecolor='black', label='Pick Object')
    place_item = patches.Rectangle((place_pos[0]-10, place_pos[1]-5), 20, 10, 
                                    facecolor='gray', edgecolor='black', alpha=0.5, label='Place Spot')
    ax4.add_patch(pick_item)
    ax4.add_patch(place_item)
    
    # Draw trajectory
    t1_pick, t2_pick = get_angles(*pick_pos)
    t1_place, t2_place = get_angles(*place_pos)
    
    # Safe path (arc above objects)
    theta_start = math.radians(t1_pick + t2_pick/2)
    theta_end = math.radians(t1_place + t2_place/2)
    
    # Draw path
    theta_vals = np.linspace(theta_start, theta_end + math.pi, 50)
    path_x = np.cos(theta_vals) * 120
    path_y = np.sin(theta_vals) * 120 + 80
    ax4.plot(path_x, path_y, 'b--', alpha=0.5, linewidth=2, label='Safe Path')
    
    # Draw robot at pick position
    j1_x, j1_y = L1 * math.cos(math.radians(t1_pick)), L1 * math.sin(math.radians(t1_pick))
    e_x = j1_x + L2 * math.cos(math.radians(t1_pick + t2_pick))
    e_y = j1_y + L2 * math.sin(math.radians(t1_pick + t2_pick))
    
    ax4.plot(0, 0, 'ko', markersize=12)
    ax4.plot([0, j1_x], [0, j1_y], 'b-', linewidth=8, label='Robot Position')
    ax4.plot([j1_x, e_x], [j1_y, e_y], 'g-', linewidth=8)
    ax4.plot(e_x, e_y, 'ro', markersize=10)
    
    # Annotations
    ax4.annotate('1️⃣ Approach\n(above pick)', xy=(pick_pos[0]+20, pick_pos[1]+30), fontsize=9)
    ax4.annotate('2️⃣ Grab\n(at height)', xy=(pick_pos[0]-50, pick_pos[1]+20), fontsize=9)
    ax4.annotate('3️⃣ Rise\n(to safe height)', xy=(20, 120), fontsize=9)
    ax4.annotate('4️⃣ Move\n(to safe height)', xy=(0, 160), fontsize=9)
    ax4.annotate('5️⃣ Lower\n(at place)', xy=(place_pos[0]-50, place_pos[1]+30), fontsize=9)
    
    # Steps box
    steps_text = '''📋 Pick & Place Steps:

1. Move above pick position (Z=safe)
2. Lower to pick height
3. Close gripper (grab)
4. Rise to safe height
5. Move to place position
6. Lower to place height
7. Open gripper (release)
8. Rise and return home'''
    
    ax4.text(0.02, 0.02, steps_text, transform=ax4.transAxes, fontsize=9,
            verticalalignment='bottom',
            bbox=dict(boxstyle='round', facecolor='lightgreen', alpha=0.9))
    
    ax4.legend(loc='upper right')
    
    plt.tight_layout()
    plt.savefig('/workspace/project/scara_diagram.png', dpi=150, bbox_inches='tight')
    print("✅ Saved: scara_diagram.png")
    plt.show()


def interactive_kinematics():
    """Interactive tool to test kinematics"""
    
    print("\n" + "=" * 60)
    print("🎮 INTERACTIVE SCARA KINEMATICS CALCULATOR")
    print("=" * 60)
    
    print("""
    This tool lets you experiment with SCARA kinematics!
    
    Options:
    1. Forward Kinematics: Given angles → Find position
    2. Inverse Kinematics: Given position → Find angles
    3. Workspace Analysis: See reachable area
    4. Joint Space Demo: Visualize joint movements
    """)
    
    # Quick demo
    L1, L2 = 100, 80
    
    print("\n📊 FORWARD KINEMATICS EXAMPLES:")
    print("-" * 40)
    
    test_angles = [
        (0, 0),
        (90, 0),
        (0, 90),
        (45, 45),
        (-45, 90),
    ]
    
    print(f"{'θ1':>6} {'θ2':>6} {'→ X':>10} {'→ Y':>10}")
    for t1, t2 in test_angles:
        t1_rad, t2_rad = math.radians(t1), math.radians(t2)
        x = L1 * math.cos(t1_rad) + L2 * math.cos(t1_rad + t2_rad)
        y = L1 * math.sin(t1_rad) + L2 * math.sin(t1_rad + t2_rad)
        print(f"{t1:>5}° {t2:>5}° {x:>10.1f}mm {y:>10.1f}mm")
    
    print("\n📊 INVERSE KINEMATICS EXAMPLES:")
    print("-" * 40)
    
    test_targets = [
        (180, 0),
        (100, 100),
        (50, 50),
        (0, 170),
    ]
    
    print(f"{'Target X':>10} {'Target Y':>10} {'→ θ1':>10} {'→ θ2':>10} {'✓ OK?':>6}")
    for tx, ty in test_targets:
        D = math.sqrt(tx**2 + ty**2)
        if D > L1 + L2 or D < abs(L1 - L2):
            print(f"{tx:>10.1f} {ty:>10.1f} {'---':>10} {'---':>10} {'✗':>6}")
        else:
            cos_t2 = (L1**2 + L2**2 - D**2) / (2 * L1 * L2)
            t2 = math.degrees(math.acos(cos_t2))
            alpha = math.degrees(math.atan2(ty, tx))
            beta = math.degrees(math.atan2(L2 * math.sin(math.radians(t2)), 
                                           L1 + L2 * math.cos(math.radians(t2))))
            t1 = alpha - beta
            print(f"{tx:>10.1f} {ty:>10.1f} {t1:>10.1f}° {t2:>10.1f}° {'✓':>6}")


if __name__ == "__main__":
    draw_scara_robot_diagram()
    interactive_kinematics()