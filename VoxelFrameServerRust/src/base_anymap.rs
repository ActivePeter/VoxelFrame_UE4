use std::any::{Any, TypeId};
use std::collections::HashMap;

pub struct AnyMap(HashMap<TypeId, Box<dyn Any + Send + Sync>>);

impl AnyMap {
    pub fn new() -> Self {
        Self(HashMap::new())
    }

    pub fn insert<T: Any + Send + Sync>(&mut self, x: T) -> Option<T> {
        self.0
            .insert(TypeId::of::<T>(), Box::new(x))
            .map(force_downcast)
    }
    pub fn remove<T: Any + Send + Sync>(&mut self) -> Option<T> {
        self.0.remove(&TypeId::of::<T>()).map(force_downcast)
    }

    pub fn get<T: Any + Send + Sync>(&self) -> Option<&T> {
        self.0
            .get(&TypeId::of::<T>())
            .map(|b| b.downcast_ref::<T>().unwrap())
    }

    pub fn get_mut<T: Any + Send + Sync>(&mut self) -> Option<&mut T> {
        self.0
            .get_mut(&TypeId::of::<T>())
            .map(|b| b.downcast_mut::<T>().unwrap())
    }
}

fn force_downcast<T: 'static>(b: Box<dyn Any + Send + Sync>) -> T {
    *<Box<dyn Any + Send>>::downcast::<T>(b).unwrap()
}