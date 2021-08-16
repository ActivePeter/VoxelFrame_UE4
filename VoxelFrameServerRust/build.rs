fn main() {
    protobuf_codegen_pure::Codegen::new()
        .out_dir("src/protos")
        .inputs(&["protos/small-model.proto"])
        .include("protos")
        .run()
        .expect("Codegen failed.");
}