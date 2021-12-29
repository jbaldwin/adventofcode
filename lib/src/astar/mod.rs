// use std::collections::{BinaryHeap, HashSet};
// use core::hash::Hash;
// use std::rc::Rc;
// use core::borrow::Borrow;

// pub enum State {
//     Searching,
//     Found,
//     Failed
// }

// pub trait AStarNodeTrait {
//     fn movement_cost(&self) -> i64;
//     fn estimated_cost(&self) -> i64;
//     fn is_goal(&self) -> bool;
//     fn children(&self) -> Option<Vec<Rc<dyn AStarNodeTrait>>>;
//     fn parent(&mut self, parent: Rc<dyn AStarNodeTrait>);
// }

// // #[derive(Clone, Eq, PartialEq, Ord, PartialOrd, Hash)]

// pub struct AStar<T>
// where
//     T: AStarNodeTrait + Clone + Eq + PartialEq + Ord + PartialOrd + Hash + 'static,
//     Rc<T>: Hash + Eq + Borrow<Rc<dyn AStarNodeTrait>>
// {
//     open_list: BinaryHeap<Rc<T>>,
//     closed_list: HashSet<Rc<T>>,
//     current: Rc<T>,
// }

// impl<T> AStar<T>
// where
//     T: AStarNodeTrait + Clone + Eq + PartialEq + Ord + PartialOrd + Hash + 'static,
//     Rc<T>: Hash + Eq + Borrow<Rc<dyn AStarNodeTrait>>
// {
//     pub fn new(start: Rc<T>) -> Self {
//         let mut open_list = BinaryHeap::new();
//         open_list.push(start.clone());
//         Self {
//             open_list,
//             closed_list: HashSet::new(),
//             current: start,
//         }
//     }

//     pub fn run(&mut self) -> State {

//         loop {
//             let state = self.step();

//             match state {
//                 State::Searching => {
                    
//                 },
//                 State::Found => {
//                     return State::Found;
//                 },
//                 State::Failed => {
//                     return State::Failed;
//                 }
//             };
//         }
//     }

//     fn step(&mut self) -> State {
//         loop {

//             // If there are no more nodes to search, return failure.
//             if self.open_list.len() == 0 {
//                 return State::Failed;
//             }

//             self.current = self.open_list.pop().unwrap();

//             if let Some(_) = self.closed_list.get(&self.current) {
//                 continue;
//             }

//             // An unsearched node has been found, lets follow it.
//             break;
//         }

//         self.closed_list.insert(self.current.clone());

//         if self.current.is_goal() {
//             return State::Found;
//         }
        
//         if let Some(children) = self.current.children() {
//             for child in children.iter() {
//                 // Skip any children already on the closed list.
//                 if let Some(_) = self.closed_list.get(child) {
//                     continue;
//                 }


//             }
//         }

//         return State::Failed;
        
//     }
// }