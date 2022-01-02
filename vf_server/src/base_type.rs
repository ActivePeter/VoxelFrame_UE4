use arrayvec::ArrayVec;

pub type Point3f = ArrayVec<f32, 3>;

pub fn point3f_new()->Point3f{
    let mut p =Point3f::new();
    p.push(0.0);
    p.push(0.0);
    p.push(0.0);
    return p;
}
