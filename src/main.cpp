#include "main.h"

pros::MotorGroup Left({-6, 11, -2}, pros::MotorGearset::green);
pros::MotorGroup Right({7, -3, 1}, pros::MotorGearset::green);
pros::Motor belt(10);
pros::Controller master(pros::E_CONTROLLER_MASTER);
pros::adi::Pneumatics clampPneumatics('H', false);
pros::adi::Pneumatics doinkerPneumatics('B', false);

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");

	pros::lcd::register_btn1_cb(on_center_button);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {

	double slow = 1;
	bool pneumaticsBool = false;
	bool pressingBool = false;
	bool doinker = false;

	while (true) {
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);  // Prints status of the emulated screen LCDs

		// Arcade control scheme
		int dir = master.get_analog(ANALOG_LEFT_Y);    // Gets amount forward/backward from left joystick
		int turn = master.get_analog(ANALOG_RIGHT_X);  // Gets the turn left/right from right joystick
		Left.move(dir + turn);                      // Sets left motor voltage
		Right.move(dir - turn);                     // Sets right motor voltage

		if (master.get_digital(DIGITAL_UP)){
			slow = 0.4;
		} else {
			slow = 1;
		}

		if (master.get_digital_new_press(DIGITAL_X)) {
			  doinkerPneumatics.extend();
			  doinkerPneumatics.retract();
		  }


		  if (master.get_digital(DIGITAL_DOWN)) {
			Left.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
			Left.brake();
			Right.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
			Right.brake();
		  } else {
			Left.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
			Left.brake();
			Right.set_brake_mode(pros::E_MOTOR_BRAKE_COAST);
			Right.brake();
		  }


		  if (master.get_digital(DIGITAL_R1)) {
			belt.move(127);
		  } else if (master.get_digital(DIGITAL_R2)) {
			belt.move(-127);
		  } else {
			belt.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
			belt.brake();
		  }

		  if (master.get_digital_new_press(DIGITAL_R1)) {
				clampPneumatics.toggle();
		  }

		pros::delay(20);                               // Run for 20 ms then update
	}
}