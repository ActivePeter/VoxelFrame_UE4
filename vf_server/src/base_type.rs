use arrayvec::ArrayVec;

pub type Point3f = ArrayVec<f32, 3>;
pub struct Point2i{
    pub x:i32,
    pub y:i32
}
impl Point2i{
    pub fn new(x:i32, y:i32) -> Point2i {
        Point2i{
            x,y
        }
    }
}
pub struct Point3i{
    pub x:i32,
    pub y:i32,
    pub z:i32,
}
impl Point3i{
    pub fn new(x:i32, y:i32, z:i32) -> Point3i {
        return Point3i{
            x,y,z
        }
    }
}
pub fn point3f_new()->Point3f{
    let mut p =Point3f::new();
    p.push(0.0);
    p.push(0.0);
    p.push(0.0);
    return p;
}

pub fn point3f_new2(x:f32, y:f32, z:f32) -> Point3f {
    let mut p =Point3f::new();
    p.push(x);
    p.push(y);
    p.push(z);
    return p;
}

