#ifndef CONSTANT_H
#define CONSTANT_H

namespace check_system {

const static int kLaserGpioNumber = 223;
const static int kLcdGpioNumber = 255;
const static int kCmosGpioNumber = 138;
const static int kErrorGpioNumber = 254;

const static bool kLaserGpioReverse = true;
const static bool kLcdGpioReverse = true;
const static bool kComsGpioReverse = true;
const static bool kErrorGpioReverse = true;

const static int kRegisterButtonNumber = 107;
const static int kAuthButtonNumber = 171;
const static int kInterruptButtonNumber = 98;
const static int kCheckSelfButtonNumber = 165;

const static int kRs485CtlGpio = 164;

const static char* const kLaserAddr = "/dev/ttyUSB0";
//const static char* const kLaserAddr = "/dev/ttyS0";
const static char* const kHostAddr = "/dev/ttyS1";

const static double kAuthThreshold = 0.5;

}
#endif // CONSTANT_H
