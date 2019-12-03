#ifndef __CONTAINERS_H
#define __CONTAINERS_H

#include <stdint.h>
#include <string.h>

#define EXTERNAL

/**
 * Sometimes the compiler would like to be smart, if we do not reference
 * anything here
 * and you enable the rightflags, the linker drops the dependency. Than
 * Proxy/Stubs do
 * not get loaded, so lets make the instantiation of the ProxyStubs explicit !!!
 */
extern "C" {
EXTERNAL void ForceLinkingOfOpenCDM();
}
#else
#define EXTERNAL
#endif

#ifdef __cplusplus

extern "C" {

#endif

struct Container_t;

/**
 * OpenCDM error code. Zero always means success.
 */
typedef enum {
    ERROR_NONE = 0,
    ERROR_UNKNOWN = 1,
    ERROR_MORE_DATA_AVAILBALE=2,
    ERROR_OUT_OF_BOUNDS = 3,
    ERROR_INVALID_KEY = 4
} ContainerError;

struct ContainerMemory {
    uint64_t allocated; // in bytes
    uint64_t resident; // in bytes
    uint64_t shared; // in bytes
};

/**
 * \brief Initializes a container.
 * This function initializes a container and prepares it to be started
 * 
 * \param container - (output) Container that is to be initialized
 * \param name - Name of started container
 * \param searchpaths - Null-terminated list of locations where container can be located. 
 *                      List is processed in the order it is provided, and the first 
 *                      container matching provided name will be selected & initialized
 * \return Zero on success, non-zero on error.
 */
EXTERNAL ContainerError container_create(struct Container_t** container, const char* name, const char** searchpaths, const char* logPath, const char* configuration);

/**
 * \brief Enables logging.
 * This functions setups logging of containers
 * 
 * \param logpath - Path to folder where the logging will be stored
 * \param logId - Null-terminated list of locations where container can be located. 
 *                      List is processed in the order it is provided, and the first 
 *                      container matching provided name will be selected & initialized
 * \param loggingOptions - Logging configuration
 * \return Zero on success, non-zero on error.
 */
EXTERNAL ContainerError container_enableLogging(const char* logpath, const char* logId, const char* loggingOptions);

/**
 * \brief Releases a container.
 * This function releases all resources taken by a container
 * 
 * all resources taken by it
 * \param container - Container that will be released
 * \return Zero on success, non-zero on error.
 */
EXTERNAL ContainerError container_release(struct Container_t* container);

/**
 * \brief Starts a container.
 * This function starts a given command in the container
 * 
 * \param container - Container that is to be initialized
 * \param command - Command that will be started in container's shell
 * \param params - List of parameters provied to command
 * \return Zero on success, non-zero on error.
 */
EXTERNAL ContainerError container_start(struct Container_t* container, const char* command, const char** params, uint32_t numParams);

/**
 * \brief Stops a container.
 * This function stops a container
 * 
 * \param container - Container that is to be initialized
 * \return Zero on success, non-zero on error.
 */
EXTERNAL ContainerError container_stop(struct Container_t* container);

/**
 * \brief Gives information if the container is running.
 *
 * This function can be used to check if container is in stopped state
 * or in running state.
 * 
 * \param container - Container that is checked
 * \return 1 if is running, 0 otherwise.
 */
EXTERNAL uint8_t container_isRunning(struct Container_t* container);

/**
 * \brief Information about memory usage.
 *
 * Function gives information about memory allocated to 
 * runnning the container
 * 
 * \param container - Container that is checked
 * \param memory - Pointer to structure that will be filled with memory 
 * information
 * \return Zero on success, error-code otherwise
 */
EXTERNAL ContainerError container_getMemory(struct Container_t* container, ContainerMemory* memory);

/**
 * \brief Information about cpu usage.
 * Function gives information about how much of CPU time was 
 * used for the container
 * 
 * \param container - Container that is checked
 * \param threadNum - Ordinal number of thread of which usage will be returned. 
 *                    If -1 is provided, total CPU usage will be reported. 
 * \param usage - Usage of cpu time in nanoseconds
 * \return Zero on success, error-code otherwise
 */
EXTERNAL ContainerError container_getCpuUsage(struct Container_t* container, int32_t threadNum, uint64_t* usage);


/**
 * \brief Number of network interfaces assigned to container.
 * Function gives information about how many network interface
 * the container have
 * 
 * \param container - Container that is checked
 * \param numNetworks - (output) Number of network interface assigned to a container.
 *  
 * \return Zero on success, error-code otherwise
 */
EXTERNAL ContainerError container_getNumNetworkInterfaces(struct Container_t* container, uint32_t* numNetworks);

/**
 * \brief Containers's network interfaces.
 * Function gives information about memory allocated to 
 * runnning the container
 * 
 * \param container - Container that is checked
 * \param interfaceNum - Ordinal number of interface. Can take value from 0 to value returned by  
 *                       container_getNumNetworkInterfaces - 1. Otherwise ERROR_OUT_OF_BOUNDS is returned
 * \param name - (output) Name of network interface
 * \param maxNameLength - Maximum length of name buffer.
 * 
 * \return Zero on success, error-code otherwise
 */
EXTERNAL ContainerError container_getNetworkInterfaceName(struct Container_t* container, uint32_t interfaceNum, char* name, uint32_t maxNameLength = 16);

/**
 * \brief Number of IP addresses asssigend to interface.
 * Function gives information about how many ip addresses are assigned to a  
 * given network interface
 * 
 * \param container - Container that is checked
 * \param interfaceName - Name of network interface on which we are checking ip. 
 *                        If set to NULL, function will return number of all ip addresses of container
 * \param numIPs - (output) Number of ips asssigned to network interface
 * 
 * \return Zero on success, error-code otherwise
 */
EXTERNAL ContainerError container_getNumIPs(struct Container_t* container, const char* interfaceName, uint32_t* numIPs);

/**
 * \brief List of IP addresses given to a container.
 * Function gives information about memory allocated to 
 * runnning the container
 * 
 * \param container - Container that is checked
 * \param interfaceName - Name of network interface on which we are checking ip. 
 *                        If set to NULL, function will return addresses from of 
 *                        all container's addresses
 * \param addressNum - Ordinal number of ip assigned to interface. Can range from 0 to value obtained from
 *                     container_getNumIPs() for a given interface. If other address is given, 
 *                     ERROR_OUT_OF_BOUNDS is returned
 * \param address - (output) IP Address of container
 * \param maxAddressLength - Maximum length of address. If address length is higher than maxAddressLength,
 *                           ERROR_MORE_DATA_AVAILABLE is returned
 * 
 * \return Zero on success, error-code otherwise
 */
EXTERNAL ContainerError container_getIP(struct Container_t* container, const char* interfaceName, uint32_t addressNum, char* address, uint32_t maxAddressLength);

/**
 * \brief Container's config path.
 * Function gives path of configuration that was used to  
 * initialize container
 * 
 * \param container - Container that is checked
 * \param path - Buffer where the containers config path will be placed
 * \param maxPathLength - Maximum length of path. If path is longer than 
 *                        this, ERROR_MORE_DATA will be returned
 * 
 * \return Zero on success, error-code otherwise
 */
EXTERNAL ContainerError container_getConfigPath(struct Container_t* container, char* path, uint32_t maxPathLength);

/**
 * \brief Containers name.
 * Function gives name of the container
 * 
 * \param container - Container that is checked
 * \param id - Name of the container
 * \param maxIdLength - Maximum length of containers name. If name is longer than 
 *                      this, ERROR_MORE_DATA will be returned
 * 
 * \return Zero on success, error-code otherwise
 */
EXTERNAL ContainerError container_getName(struct Container_t* container, char* name, uint32_t maxNameLength);

#ifdef __cplusplus
}
#endif

#endif // __CONTAINERS_H
