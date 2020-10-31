compile:
	arduino-cli compile --fqbn arduino:avr:nano328p src --verbose
upload:
	arduino-cli upload --port /dev/ttyUSB0 --fqbn arduino:avr:nano328p src --verbose
