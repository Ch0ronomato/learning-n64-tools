struct camera_opts { float fov; float near; float far; float pos[3];};
static struct camera_opts gCamera = 
{
    .fov = 45.0,
    .near = 2.000000,
    .far = 40.000000,
    .pos = {100.000000, 25.000000, 0.000000}
};
