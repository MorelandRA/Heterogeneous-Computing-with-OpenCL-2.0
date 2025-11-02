#include <stdio.h>
#include <CL/cl.h>

static void* force_malloc(size_t size);

static cl_int retrieve_devices(cl_platform_id platform, cl_uint *device_count_ret, cl_device_id **device_ids_ret);
static cl_int count_devices(cl_platform_id platform, cl_uint *device_count_ret);
static void print_devices(cl_uint device_count,const cl_device_id *device_ids);
static void print_platform_devices(cl_platform_id platform);

static cl_int retrieve_platforms(cl_uint *platform_count_ret, cl_platform_id **platform_ids_ret);
static cl_int count_platforms(cl_uint *platform_count_ret);
static void print_platforms(cl_uint platform_count, const cl_platform_id *platform_ids);

int main()
{
    cl_platform_id *platforms = NULL;
    cl_uint platform_count;

    cl_int status = retrieve_platforms(&platform_count, &platforms);
    if (status != CL_SUCCESS || platform_count == 0)
    {
        fprintf(stderr, "No platforms were found, status code:%d\n",status);
        exit(EXIT_FAILURE);
    }

    print_platforms(platform_count, platforms);

    printf("\n");

    for (cl_uint platform_num = 0; platform_num < platform_count; platform_num++)
    {
        cl_platform_id platform = platforms[platform_num];
        print_platform_devices(platform);
    }

    free(platforms);
    return EXIT_SUCCESS;
}

static void* force_malloc(size_t size)
{
    void *response = malloc(size);
    if (!response)
    {
        fprintf(stderr, "Failed to allocate %zu bytes of memory\n", size);
        perror("force_malloc");
        exit(EXIT_FAILURE);
    }
    return response;
}

static cl_int retrieve_devices(cl_platform_id platform, cl_uint *device_count_ret, cl_device_id **device_ids_ret)
{
    cl_int status = count_devices(platform, device_count_ret);
    if (status != CL_SUCCESS || *device_count_ret == 0) return status;

    *device_ids_ret = force_malloc(sizeof(cl_device_id) * (*device_count_ret));

    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, *device_count_ret, *device_ids_ret, NULL);
    if (status != CL_SUCCESS)
    {
        fprintf(stderr, "Failed to retrieve devices on platform %p, status code: \"%d\"\n", platform, status);
        return status;
    }
    return status;
}

static cl_int count_devices(cl_platform_id platform, cl_uint *device_count_ret)
{
    cl_int status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, device_count_ret);
    if (status != CL_SUCCESS)
        fprintf(stderr, "Failed to count devices on platform %p, status code: \"%d\"\n", platform, status);
    else
        printf("Found %u device(s) on Platform %p\n", *device_count_ret, platform);
    return status;
}

static void print_devices(cl_uint device_count, const cl_device_id *device_ids)
{
    for (cl_uint device_num = 0; device_num < device_count; device_num++)
    {
        size_t name_malloc_size;
        cl_int status = clGetDeviceInfo(device_ids[device_num], CL_DEVICE_NAME, 0, NULL, &name_malloc_size);
        if (status != CL_SUCCESS)
        {
            fprintf(stderr, "Failed to get size of name for device %u, status code: \"%d\"\n", device_num, status);
            continue;
        }
        char *name = force_malloc(name_malloc_size);

        status = clGetDeviceInfo(device_ids[device_num], CL_DEVICE_NAME, name_malloc_size, name, NULL);
        if (status != CL_SUCCESS) fprintf(stderr, "Failed to get name for device %u, status code: \"%d\"\n", device_num, status);
        else printf("\tDevice %u: %s (%p)\n", device_num, name, device_ids[device_num]);
        free(name);
    }
}

static void print_platform_devices(cl_platform_id platform)
{
    cl_device_id *devices = NULL;
    cl_uint device_count;
    cl_int status = retrieve_devices(platform, &device_count, &devices);
    if (status == CL_SUCCESS)
    {
        print_devices(device_count, devices);
    }

    free(devices);
}

static cl_int retrieve_platforms(cl_uint *platform_count_ret, cl_platform_id **platform_ids_ret)
{
    cl_int status = count_platforms(platform_count_ret);
    if (status != CL_SUCCESS || *platform_count_ret == 0) return status;

    *platform_ids_ret = force_malloc(sizeof(cl_platform_id) * (*platform_count_ret));

    status = clGetPlatformIDs(*platform_count_ret, *platform_ids_ret, NULL);

    if (status != CL_SUCCESS)
    {
        fprintf(stderr, "Failed to retrieve platforms, status code: \"%d\"\n", status);
    }
    return status;
}

static cl_int count_platforms(cl_uint *platform_count_ret)
{
    cl_int status = clGetPlatformIDs(0, NULL, platform_count_ret);

    if (status != CL_SUCCESS)
        fprintf(stderr, "Failed to retrieve platforms, status code: \"%d\"\n", status);
    else
        printf("Found %u platform(s)\n", *platform_count_ret);

    return status;
}

static void print_platforms(cl_uint platform_count, const cl_platform_id *platform_ids)
{
    for (cl_uint platform_num = 0; platform_num < platform_count; platform_num++)
    {
        size_t name_malloc_size;
        cl_int status = clGetPlatformInfo(platform_ids[platform_num], CL_PLATFORM_NAME, 0, NULL, &name_malloc_size);

        if (status != CL_SUCCESS)
        {
            fprintf(stderr, "Failed to get size of name for platform %u, status code: \"%d\"\n", platform_num, status);
            continue;
        }
        char *name = force_malloc(name_malloc_size);

        status = clGetPlatformInfo(platform_ids[platform_num], CL_PLATFORM_NAME, name_malloc_size, name, NULL);

        if (status != CL_SUCCESS) fprintf(stderr, "Failed to get name for platform %u, status code: \"%d\"\n", platform_num, status);
        else printf("\tPlatform %u: %s (%p)\n", platform_num, name, platform_ids[platform_num]);

        free(name);
    }
}
