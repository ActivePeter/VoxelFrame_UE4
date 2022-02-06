use arrayvec::ArrayVec;

pub type Point3f = ArrayVec<f32, 3>;

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