#!/usr/bin/env python3
"""
================================================================
SCARA ROBOT PROGRAMMING COURSE: FROM ZERO TO HERO
================================================================

A Layman's Guide to Understanding & Programming a 2-Arm SCARA Robot

This course teaches you how SCARA robots work and how to program them
from scratch. Perfect for beginners with no robotics experience!

================================================================
PART 1: WHAT IS A SCARA ROBOT?
================================================================

SCARA stands for "Selective Compliance Assembly Robot Arm"

It's an industrial robot with:
- 2 rotary arms (like upper arm + forearm)
- 1 vertical lift (Z-axis up/down movement)  
- End effector (gripper/claw at the tip)

Structure diagram:
        [GRIPPER]  <- End Effector
            |
        [ARM 2]    <- Forearm (L2)
            |
        [ARM 1]    <- Upper Arm (L1)
            |
         [BASE]

Key Features:
1. Fast & Precise - parallel axis design
2. Rigid in Z-axis - good for vertical insertion
3. Simple 4-axis control: theta1, theta2, Z, gripper

================================================================
PART 2: FORWARD KINEMATICS
================================================================

"Given angles -> Where is my hand?"

The robot has two joints with angles theta1 and theta2.
Given these angles and arm lengths L1, L2, we calculate X, Y position.

FORMULA:
    X = L1 * cos(theta1) + L2 * cos(theta1 + theta2)
    Y = L1 * sin(theta1) + L2 * sin(theta1 + theta2)

Example:
    If L1=100mm, L2=80mm, theta1=45deg, theta2=30deg
    X = 100*cos(45) + 80*cos(75) = 70.7 + 20.7 = 91.4mm
    Y = 100*sin(45) + 80*sin(75) = 70.7 + 77.3 = 148mm
"""

import math


def forward_kinematics(L1, L2, theta1_deg, theta2_deg):
    """
    Calculate X, Y position from joint angles
    
    Args:
        L1: Length of first arm (mm)
        L2: Length of second arm (mm)
        theta1_deg: First joint angle (degrees)
        theta2_deg: Second joint angle (degrees)
    
    Returns:
        (X, Y) position in mm
    """
    # Convert degrees to radians (Python uses radians for trig)
    theta1 = math.radians(theta1_deg)
    theta2 = math.radians(theta2_deg)
    
    # Calculate end effector position using trigonometry
    X = L1 * math.cos(theta1) + L2 * math.cos(theta1 + theta2)
    Y = L1 * math.sin(theta1) + L2 * math.sin(theta1 + theta2)
    
    return X, Y


def lesson1_forward_kinematics():
    """Lesson 1: Learn how angles affect position"""
    print("\n" + "="*60)
    print("LESSON 1: Forward Kinematics (Angles -> Position)")
    print("="*60)
    
    L1, L2 = 100, 80  # Arm lengths in mm
    
    print(f"\nRobot specs: L1={L1}mm, L2={L2}mm")
    print("\nTrying different joint angles:\n")
    print(f"{'Theta1':>8} {'Theta2':>8} {'X Position':>12} {'Y Position':>12}")
    print("-" * 45)
    
    test_cases = [
        (0, 0),      # Straight right
        (90, 0),     # Pointing up
        (0, 90),     # Elbow bent
        (45, 45),    # Diagonal
        (45, -45),   # Elbow bent back
        (90, 90),    # Folded up
        (-90, 90),   # Pointing left
    ]
    
    for t1, t2 in test_cases:
        x, y = forward_kinematics(L1, L2, t1, t2)
        print(f"{t1:>7}deg {t2:>7}deg {x:>12.1f}mm {y:>12.1f}mm")
    
    print("\n[Try changing the angles in the code to experiment!]")


"""
================================================================
PART 3: INVERSE KINEMATICS
================================================================

"The Hard Part: Given X, Y -> Find angles"

This is more complex! We need to solve for theta1 and theta2
given the target position.

STEPS:
1. Calculate distance D from base to target
2. Check if target is reachable (within workspace)
3. Use Law of Cosines to find theta2
4. Use atan2 to find theta1

FORMULA (Law of Cosines):
    theta2 = arccos((L1^2 + L2^2 - D^2) / (2 * L1 * L2))
    
    where D = distance from base to target

FORMULA (theta1):
    theta1 = atan2(Y, X) - atan2(L2*sin(theta2), L1 + L2*cos(theta2))

WORKSPACE:
    - Maximum reach: L1 + L2 (180mm in our case)
    - Minimum reach: |L1 - L2| (20mm in our case)
"""


def inverse_kinematics(L1, L2, target_x, target_y):
    """
    Calculate joint angles to reach a target position
    
    Uses the geometric (analytic) approach.
    
    Formula derivation:
    - theta2: From law of cosines, cos(theta2) = (D^2 - L1^2 - L2^2) / (2 * L1 * L2)
    - theta1: The elbow angle psi - base_angle, where base_angle comes from
              sin(base_angle)/L2 = sin(theta2)/D
    
    Args:
        L1: Length of first arm (mm)
        L2: Length of second arm (mm)
        target_x: Desired X position (mm)
        target_y: Desired Y position (mm)
    
    Returns:
        (theta1, theta2) in degrees, or None if unreachable
    """
    # Calculate distance from base to target
    D = math.sqrt(target_x**2 + target_y**2)
    
    # Check if reachable (with small epsilon for floating point)
    max_reach = L1 + L2 - 0.001
    min_reach = abs(L1 - L2) + 0.001
    
    if D > max_reach:
        print(f"  Target too far! D={D:.1f}mm > max={max_reach:.1f}mm")
        return None
    if D < min_reach:
        print(f"  Target too close! D={D:.1f}mm < min={min_reach:.1f}mm")
        return None
    
    # psi = angle from base to target
    psi = math.atan2(target_y, target_x)
    
    # theta2 = elbow angle
    # cos(theta2) = (D^2 - L1^2 - L2^2) / (2 * L1 * L2)
    cos_theta2 = (D**2 - L1**2 - L2**2) / (2 * L1 * L2)
    cos_theta2 = max(-1.0, min(1.0, cos_theta2))
    theta2 = math.acos(cos_theta2)
    
    # base_angle = angle between L1 and the line to target
    # sin(base_angle)/L2 = sin(theta2)/D
    base_angle = math.asin(max(-1.0, min(1.0, L2 * math.sin(theta2) / D)))
    
    # theta1 = psi - base_angle
    theta1 = psi - base_angle
    
    return math.degrees(theta1), math.degrees(theta2)


def lesson2_inverse_kinematics():
    """Lesson 2: Learn how to reach any position"""
    print("\n" + "="*60)
    print("LESSON 2: Inverse Kinematics (Position -> Angles)")
    print("="*60)
    
    L1, L2 = 100, 80
    
    print(f"\nRobot specs: L1={L1}mm, L2={L2}mm")
    print(f"Workspace: {abs(L1-L2)}mm to {L1+L2}mm")
    print("\nFinding angles to reach positions:\n")
    
    print(f"{'Target X':>10} {'Target Y':>10} {'Theta1':>10} {'Theta2':>10} {'Result':>10}")
    print("-" * 55)
    
    test_targets = [
        (180, 0),    # Straight right
        (100, 100),  # Diagonal
        (0, 170),    # Straight up
        (50, 50),    # Close diagonal
        (20, 0),     # At minimum reach
        (170, 0),    # At maximum reach
    ]
    
    for tx, ty in test_targets:
        result = inverse_kinematics(L1, L2, tx, ty)
        if result:
            t1, t2 = result
            # Verify by doing forward kinematics
            x, y = forward_kinematics(L1, L2, t1, t2)
            error = math.sqrt((x-tx)**2 + (y-ty)**2)
            status = 'OK' if error < 0.1 else f'Error({error:.2f}mm)'
            print(f"{tx:>10.1f} {ty:>10.1f} {t1:>10.1f}deg {t2:>10.1f}deg {status:>15}")
        else:
            print(f"{tx:>10.1f} {ty:>10.1f} {'---':>10} {'---':>10} {'UNREACHABLE':>15}")

    print("\n[Key insight: Robot can only reach within an annular workspace!]")


"""
================================================================
PART 4: BUILDING A SCARA ROBOT CLASS
================================================================

Now let's put everything together in a proper robot class!
"""


class SCARARobot:
    """
    Simulates a 2-arm SCARA robot
    
    This is the core class that models the robot.
    You can use this to:
    - Test trajectories
    - Plan movements
    - Simulate before real hardware
    """
    
    def __init__(self, L1=100, L2=80, base_x=0, base_y=0):
        """
        Initialize SCARA robot
        
        Args:
            L1: Upper arm length (mm)
            L2: Forearm length (mm)
            base_x: Base X position (mm)
            base_y: Base Y position (mm)
        """
        self.L1 = L1
        self.L2 = L2
        self.base_x = base_x
        self.base_y = base_y
        self.theta1 = 0
        self.theta2 = 0
        self.z = 0  # Height
        self.gripper_open = True
    
    def set_angles(self, theta1, theta2):
        """Set joint angles"""
        self.theta1 = theta1
        self.theta2 = theta2
    
    def set_position(self, x, y):
        """Set end effector to target position using IK"""
        result = inverse_kinematics(self.L1, self.L2, x, y)
        if result:
            self.theta1, self.theta2 = result
            return True
        return False
    
    def get_position(self):
        """Get current X, Y position"""
        return forward_kinematics(self.L1, self.L2, self.theta1, self.theta2)
    
    def get_state(self):
        """Get complete robot state"""
        x, y = self.get_position()
        return {
            'theta1': self.theta1,
            'theta2': self.theta2,
            'x': x,
            'y': y,
            'z': self.z,
            'gripper': 'OPEN' if self.gripper_open else 'CLOSED'
        }
    
    def move_to(self, x, y, z=None):
        """
        Move to position (with optional height)
        
        This is a simplified move - real robots need trajectory planning!
        """
        if self.set_position(x, y):
            if z is not None:
                self.z = z
            return True
        return False
    
    def pick(self, x, y):
        """
        Pick up object at position
        1. Move above
        2. Lower
        3. Close gripper
        4. Rise with object
        """
        SAFE_HEIGHT = 50
        
        # Step 1: Move above
        self.move_to(x, y, SAFE_HEIGHT)
        print(f"  Move above ({x}, {y}, height={SAFE_HEIGHT})")
        
        # Step 2: Lower
        self.z = 0
        print(f"  Lower to height=0")
        
        # Step 3: Close gripper
        self.gripper_open = False
        print(f"  Close gripper - PICKED UP!")
        
        # Step 4: Rise
        self.z = SAFE_HEIGHT
        print(f"  Rise to height={SAFE_HEIGHT}")
    
    def place(self, x, y):
        """
        Place object at position
        1. Move above
        2. Lower
        3. Open gripper
        4. Rise
        """
        SAFE_HEIGHT = 50
        
        # Step 1: Move above
        self.move_to(x, y, SAFE_HEIGHT)
        print(f"  Move above ({x}, {y}, height={SAFE_HEIGHT})")
        
        # Step 2: Lower
        self.z = 0
        print(f"  Lower to height=0")
        
        # Step 3: Open gripper
        self.gripper_open = True
        print(f"  Open gripper - PLACED!")
        
        # Step 4: Rise and move to safe position
        self.z = SAFE_HEIGHT
        # Move to a reachable home position
        if x > 0:
            self.move_to(50, 0)
        else:
            self.move_to(-50, 0)
        print(f"  Rise and move to safe position")


def lesson3_robot_class():
    """Lesson 3: Using the SCARARobot class"""
    print("\n" + "="*60)
    print("LESSON 3: Building a SCARA Robot Class")
    print("="*60)
    
    # Create robot
    robot = SCARARobot(L1=100, L2=80)
    
    print("\nCreated robot with:")
    print(f"  L1 (upper arm): {robot.L1}mm")
    print(f"  L2 (forearm): {robot.L2}mm")
    
    # Test movements
    print("\nTesting robot movements:\n")
    
    test_positions = [
        (100, 50),
        (80, 120),
        (-50, 100),
        (100, 100),  # Final position instead of home
    ]
    
    for x, y in test_positions:
        if robot.move_to(x, y):
            state = robot.get_state()
            print(f"Move to ({x}, {y}):")
            print(f"  Angles: theta1={state['theta1']:.1f}deg, theta2={state['theta2']:.1f}deg")
            print(f"  Position: X={state['x']:.1f}mm, Y={state['y']:.1f}mm")
            print()
        else:
            print(f"Move to ({x}, {y}): FAILED - unreachable\n")
    
    print("[Try creating your own robot and moving it!]")


def lesson4_pick_and_place():
    """Lesson 4: Pick and place programming"""
    print("\n" + "="*60)
    print("LESSON 4: Pick and Place Sequence")
    print("="*60)
    
    robot = SCARARobot(L1=100, L2=80)
    
    PICK_POS = (100, 50)
    PLACE_POS = (-80, 80)
    
    print(f"\nPick position: {PICK_POS}")
    print(f"Place position: {PLACE_POS}")
    print("\nExecuting pick sequence:")
    print("-" * 40)
    
    robot.pick(*PICK_POS)
    
    print("\nExecuting place sequence:")
    print("-" * 40)
    
    robot.place(*PLACE_POS)
    
    print("\n" + "="*60)
    print("Pick and place complete!")
    print("="*60)


"""
================================================================
PART 5: ARDUINO HARDWARE CODE
================================================================

Now let's look at REAL code for controlling an actual SCARA robot!
"""


def lesson5_arduino_code():
    """Lesson 5: Arduino code for real hardware"""
    print("\n" + "="*60)
    print("LESSON 5: Arduino Hardware Control")
    print("="*60)
    
    arduino_code = '''
// SCARA Robot Controller for Arduino
// Copy this code to Arduino IDE and upload!

#include <AccelStepper.h>

// Define stepper motors for each joint
AccelStepper stepper1(AccelStepper::FULL4WIRE, 8, 9, 10, 11);  // Joint 1 (Shoulder)
AccelStepper stepper2(AccelStepper::FULL4WIRE, 4, 5, 6, 7);   // Joint 2 (Elbow)

// Robot dimensions (mm)
const float L1 = 100.0;  // Upper arm
const float L2 = 80.0;   // Forearm

// Current joint angles
float theta1 = 0;
float theta2 = 0;

// Steps per revolution
const float STEPS_PER_REV = 200;
const float MICROSTEPS = 16;

void setup() {
    Serial.begin(115200);
    
    // Configure motors
    stepper1.setMaxSpeed(1000);
    stepper1.setAcceleration(500);
    stepper2.setMaxSpeed(1000);
    stepper2.setAcceleration(500);
    
    homeRobot();
}

void loop() {
    // Example: Move to target
    float targetX = 100;
    float targetY = 50;
    
    if (moveToPosition(targetX, targetY)) {
        Serial.print("Reached: ");
        Serial.print(targetX);
        Serial.print(", ");
        Serial.println(targetY);
    }
    
    delay(2000);
}

// Forward Kinematics
void forwardKinematics(float t1, float t2, float &x, float &y) {
    float t1Rad = t1 * PI / 180.0;
    float t2Rad = t2 * PI / 180.0;
    
    x = L1 * cos(t1Rad) + L2 * cos(t1Rad + t2Rad);
    y = L1 * sin(t1Rad) + L2 * sin(t1Rad + t2Rad);
}

// Inverse Kinematics
bool inverseKinematics(float targetX, float targetY, float &t1, float &t2) {
    float dx = targetX;
    float dy = targetY;
    float distance = sqrt(dx*dx + dy*dy);
    
    // Check if reachable
    if (distance > L1 + L2 || distance < abs(L1 - L2)) {
        return false;
    }
    
    // Calculate theta2
    float cosTheta2 = (L1*L1 + L2*L2 - distance*distance) / (2*L1*L2);
    t2 = acos(cosTheta2) * 180.0 / PI;
    
    // Calculate theta1
    float alpha = atan2(dy, dx);
    float beta = atan2(L2*sin(t2*PI/180.0), L1 + L2*cos(t2*PI/180.0));
    t1 = (alpha - beta) * 180.0 / PI;
    
    return true;
}

bool moveToPosition(float x, float y) {
    float newT1, newT2;
    
    if (inverseKinematics(x, y, newT1, newT2)) {
        // Move motors to new position
        // (Implementation depends on your hardware)
        theta1 = newT1;
        theta2 = newT2;
        return true;
    }
    return false;
}

void homeRobot() {
    // Add homing sequence with limit switches
    theta1 = 0;
    theta2 = 0;
    stepper1.setCurrentPosition(0);
    stepper2.setCurrentPosition(0);
}
'''
    
    print("\nARDUINO CODE FOR SCARA ROBOT CONTROL")
    print("="*60)
    print(arduino_code)
    
    print("\nHardware needed:")
    print("  - Arduino Uno/Nano")
    print("  - 2x Stepper motors (NEMA 17)")
    print("  - 2x A4988 stepper drivers")
    print("  - Power supply (12V)")
    print("  - Mechanical parts (base, arms, etc.)")


"""
================================================================
PART 6: TRAJECTORY PLANNING
================================================================

Smooth motion between points using interpolation
"""


def lerp(a, b, t):
    """Linear interpolation between a and b"""
    return a + (b - a) * t


def trajectory_planning():
    """Lesson 6: Path planning and interpolation"""
    print("\n" + "="*60)
    print("LESSON 6: Trajectory Planning")
    print("="*60)
    
    robot = SCARARobot(L1=100, L2=80)
    
    # Define waypoints for a square path
    waypoints = [
        (100, 50),
        (100, 150),
        (-100, 150),
        (-100, 50),
        (100, 50),  # Return to start
    ]
    
    print("\nGenerating smooth path through waypoints:\n")
    
    # Generate interpolated path
    interpolated = []
    steps_per_segment = 10
    
    for i in range(len(waypoints) - 1):
        x1, y1 = waypoints[i]
        x2, y2 = waypoints[i + 1]
        
        for step in range(steps_per_segment):
            t = step / steps_per_segment
            x = lerp(x1, x2, t)
            y = lerp(y1, y2, t)
            interpolated.append((x, y))
    
    print(f"Original waypoints: {len(waypoints)}")
    print(f"Interpolated points: {len(interpolated)}")
    print("\nSample of interpolated path:\n")
    print(f"{'Step':>5} {'X':>10} {'Y':>10} {'Theta1':>10} {'Theta2':>10}")
    print("-" * 50)
    
    for i, (x, y) in enumerate(interpolated[::5]):  # Every 5th point
        result = inverse_kinematics(robot.L1, robot.L2, x, y)
        if result:
            t1, t2 = result
            print(f"{i*5:>5} {x:>10.1f} {y:>10.1f} {t1:>10.1f}deg {t2:>10.1f}deg")
    
    print("\n[Key concepts: linear, circular, and spline interpolation]")


"""
================================================================
MAIN MENU
================================================================
"""


def main():
    """Main menu for the course"""
    print("\n" + "="*60)
    print("🎓 SCARA ROBOT PROGRAMMING COURSE")
    print("="*60)
    
    print("""
Welcome! This course teaches you to program a 2-arm SCARA robot.

LESSONS:
  1. Forward Kinematics - Angles to position
  2. Inverse Kinematics - Position to angles  
  3. Robot Class - Complete robot model
  4. Pick and Place - Practical application
  5. Arduino Code - Real hardware control
  6. Trajectory Planning - Smooth motion
""")
    
    print("Running lessons 1-4 as demonstration...\n")
    
    lesson1_forward_kinematics()
    print()
    lesson2_inverse_kinematics()
    print()
    lesson3_robot_class()
    print()
    lesson4_pick_and_place()
    
    print("\n" + "="*60)
    print("🎉 Congratulations! You've completed the basics!")
    print("="*60)
    print("""
NEXT STEPS:
  1. Try running lesson5_arduino_code() to see hardware code
  2. Try running trajectory_planning() for motion planning
  3. Modify the code to experiment!
  4. Get a SCARA robot kit and upload the Arduino code
""")


if __name__ == "__main__":
    main()