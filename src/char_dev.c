/**
 * Mattijs Korpershoek
 * <mattijs.korpershoek@gmail.com>
 * Alexandre Montilla
 * <alexandre.montilla@gmail.com>
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include "thermOperations.h"
#include "bitOperations.h"
#include "protocolOperations.h"
#include <linux/types.h> // include added for autocompletion
#include <linux/delay.h>

/* minor aliases */
static const unsigned char NB_OF_MINORS = 1;

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

/* Module parameters */
static int GpioPort = 2;

/* Standard character device operations */
static ssize_t read(struct file *f, char *buf, size_t size, loff_t *offset);
static int open(struct inode *in, struct file *f);
static int release(struct inode *in, struct file *f);

struct priv_t 
{
  unsigned int minor;
};

/* File operations for our character device */
static struct file_operations fileOperations = 
{
  .read = read,
  .write = NULL,
  .open = open,
  .release = release
};

/* dev_t contains major and minor version */
dev_t dev;

/* struct cdev == a Character Device*/
struct cdev *myDevice;

static ssize_t read(struct file *f, char *buf, size_t size, loff_t *offset)
{
  printk(KERN_INFO "Read called!\n");

  //   sizeToRead = MIN(myLinkedList->bufferSize - myLinkedList->beginReadIndex, size); // this condition is not valid for a non destructive read
  //
  //      printk(KERN_INFO "BeginReadIndex: %ld from file\n", myLinkedList->beginReadIndex);
  //      printk(KERN_INFO "Have to read %d from file\n", sizeToRead);
  //
  //   myBuffer = (char*)kmalloc(sizeof(char)*sizeToRead, GFP_KERNEL);
  //
  //   myLinkedList->readFromLinkedList(myLinkedList, myBuffer, sizeToRead);
  //
  //   sizeNotCopiedToUser = copy_to_user(buf, myBuffer, sizeToRead);
  //   printk(KERN_INFO "Could not copy %d bytes to user\n", sizeNotCopiedToUser);
  //
  //   kfree(myBuffer);
  //
  //   return (sizeToRead-sizeNotCopiedToUser);
  return 0;
}

static int open(struct inode *in, struct file *f)
{
  int errorCode = 0;

  printk(KERN_INFO "Pid(%d) Open with (major,minor) = (%d,%d)\n", current->tgid, MAJOR(in->i_rdev), MINOR(in->i_rdev));

  return errorCode;
}

static int release(struct inode *in, struct file *f)
{
  int errorCode = 0;

  printk(KERN_INFO "Pid(%d) Release with (major,minor) = (%d,%d)\n", current->tgid, MAJOR(in->i_rdev), MINOR(in->i_rdev));

  return errorCode;
}

static void test_gpio_led()
{
  // turns on the gpio led to show something actually working
  int i;
  int ledPin = 12;
  int initValue = initializeBitOperations(ledPin);
  if (initValue == 0)
  {
    printk(KERN_INFO "Gpio initialized");
  }
  else
  {
    printk(KERN_ALERT "ERROR while calling initializeGPIO");
  }
  printk(KERN_INFO "Blinking led 3 times\n");
  for ( i = 0; i < 3; i++ )
  {
    holdBus(ONE);
    writeBitGpio(ONE);
    msleep(1000);
    releaseBus();
    msleep(1000);
  }
}

static void test_discovery_process(void)
{
  /* displays GPIO port */
  printk(KERN_INFO "GpioPort=%d\n", GpioPort);
  int initValue = initializeBitOperations(GpioPort);
  if (initValue == 0)
  {
    printk(KERN_INFO "Gpio initialized");
  }
  else
  {
    printk(KERN_ALERT "ERROR while calling initializeGPIO");
  }
  printk(KERN_INFO "Sending an initialization sequence...\n");
  sendInitializationSequence();
  writeROMCommand(SEARCH_ROM);
  performDiscovery();
}


static int init(void)
{
  int errorCode = 0;

  /* dynamic allocation for major/minors */
  if (alloc_chrdev_region(&dev, 0, NB_OF_MINORS, "sample") == -1)
  {
    printk(KERN_ALERT ">>> ERROR alloc_chrdev_region\n");
    return -EINVAL;
  }
  /* display majors/minor */
  printk(KERN_INFO "Init allocated (major, minor)=(%d,%d)\n",MAJOR(dev),MINOR(dev));


  /* allocating memory for our character device and linking fileOperations */
  myDevice = cdev_alloc();
  myDevice->ops = &fileOperations;
  myDevice->owner = THIS_MODULE;

  /* link between chrdev region and fileOperations */
  errorCode = cdev_add(myDevice,dev,NB_OF_MINORS);
  if (errorCode == -1)
  {
    printk(KERN_ALERT ">>> ERROR cdev_add\n");
    return -EINVAL;
  }

 // test_gpio_led();
  test_discovery_process();


 /* attempt to read temperature */
//   sendInitializationSequence();
//   writeROMCommand(SKIP_ROM);
//   writeFunctionCommand(CONVERT_TEMP);
 //  Bit statusConversion;
 //  do
 //  {
 //    statusConversion = readBitFromBus();
 //  }while(statusConversion != ONE); //waiting for the temperature to be fully converted to the scratchpad
 //
   //sendInitializationSequence();
   //writeROMCommand(SKIP_ROM);
   //writeFunctionCommand(READ_SCRATCHPAD);
//   int i;
// #define blah 9
//   u8 result[blah] = {0};
//   for ( i = 0; i < blah; i++ )
//   {
//     result[i] = readByteFromBus();
//   }
//     
//   for ( i = 0; i < blah; i++ )
//   {
//     printk(KERN_INFO "%x", result[i]);
//   }
 
 deleteBitOperations();

 return(errorCode);
}


static void cleanup(void)
{
  int errorCode = 0;
  deleteBitOperations();
  /* freeing memory and major,(s) */
  unregister_chrdev_region(dev,NB_OF_MINORS);
  cdev_del(myDevice);
}

module_exit(cleanup);
module_init(init);

module_param(GpioPort, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(GpioPort, "Used GPIO port for the thermometer");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mattijs Korpershoek / Alexandre Montilla");
MODULE_DESCRIPTION("1-Wire Digital thermometer DS18B2 driver");
MODULE_SUPPORTED_DEVICE("DS18B2");
