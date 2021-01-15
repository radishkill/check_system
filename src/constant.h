#ifndef CONSTANT_H
#define CONSTANT_H

namespace check_system {

const static int kLaserGpioNumber = 254;
const static int kCmosGpioNumber = 139;
const static int kLcdGpioNumber = 255;
const static int kErrorGpioNumber = 223;

const static bool kLaserGpioReverse = false;
const static bool kLcdGpioReverse = false;
const static bool kCmosGpioReverse = false;
const static bool kErrorGpioReverse = false;


const static int kAuthButtonNumber = 252;
const static int kRegisterButtonNumber = 257;
const static int kInterruptButtonNumber = 218;
const static int kCheckSelfButtonNumber = 251;

const static int kRs485CtlGpio = 164;

//const static char* const kLaserAddr = "/dev/ttyUSB0";
const static char* const kLaserAddr = "/dev/ttyS0";
const static char* const kHostAddr = "/dev/ttyS1";

const static double kAuthThreshold = 0.2;

}
#endif // CONSTANT_H
