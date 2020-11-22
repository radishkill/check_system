#ifndef CONSTANT_H
#define CONSTANT_H

namespace check_system {

const static int kLaserGpioNumber = 223;
const static int kLcdGpioNumber = 255;
const static int kCmosGpioNumber = 223;
const static int kErrorGpioNumber = 254;

const static int kRegisterButtonNumber = 107;
const static int kAuthButtonNumber = 171;
const static int kInterruptButtonNumber = 98;
const static int kCheckSelfButtonNumber = 165;

const static int kRs485CtlGpio = 164;

const static char* const kLaserAddr = "/dev/ttyUSB0";
//const static char* const kLaserAddr = "/dev/ttyUSB0";
const static char* const kHostAddr = "/dev/ttyS1";

}
#endif // CONSTANT_H
