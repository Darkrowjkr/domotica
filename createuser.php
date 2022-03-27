<?php
require_once 'conexion.php';
require_once 'jwt.php';

if($_SERVER['REQUEST_METHOD'] == 'POST'){
    if(isset($_POST["user"]) && isset($_POST["pass"])){
        $c = conexion();
        $s = $c->prepare("INSERT INTO users (user, pass, nombre, rol) VALUES( :u, :p, :n, :r)");
        $s->bindValue(":u", $_POST["user"]);
        $s->bindValue(":p", sha1($_POST["pass"]));
        $s->bindValue(":n", $_POST["nombre"]);
        $s->bindValue(":r", $_POST["rol"]);
        $s->execute();
            if($s->rowCount() > 0){
                header("http/1.1 201 created");
                echo json_encode(array("add" => "Y"));
            }else{
                header("http/1.1 400 bad request");
                echo json_encode(array("add" => "N"));
            }
    }else{
        header("http/1.1 400 bad request");
    }
}else{
    header("http/1.1 405 method not allowed");
}