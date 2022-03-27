<?php
require_once 'conexion.php';
require_once 'jwt.php';

/***********BLOQUE DE ACCESO DE SEGURIDAD */
$headers = apache_request_headers();
$tmp = $headers['Authorization'];
$jwt = str_replace("Bearer ", "", $tmp);
if(JWT::verify($jwt, Config::SECRET) > 0){
    header("http/1.1 401 Unauthorized");
    exit;
}

$user = JWT::get_data($jwt, Config::SECRET)['user'];
/*****BLOQUE WEB SERVICE REST */
$metodo = $_SERVER["REQUEST_METHOD"];
switch($metodo){
    case 'GET':
            $c = conexion();
            if(isset($_GET['id'])){
                $s = $c->prepare("SELECT * FROM registro WHERE id = :id");
                $s->bindValue(":id", $_GET['id']);
            }else{
                $s = $c->prepare("SELECT * FROM registro");
            }
            $s->execute();
            $s->setFetchMode(PDO::FETCH_ASSOC);
            $r = $s->fetchAll();
            header("http/1.1 200 ok");
            echo json_encode($r);
        break;
    case 'POST':
            if(isset($_POST['sensor']) && isset($_POST['estado'])){
                $c = conexion();
                $s = $c->prepare("INSERT INTO registro (user, sensor, ubicacion, estado, fecha) VALUES(:us, :s, :ub, :e, NOW())");
                $s->bindValue(":us", $user);
                $s->bindValue(":s", $_POST['sensor']);
                $s->bindValue(":ub", $_POST['ubicacion']);
                $s->bindValue(":e", $_POST['estado']);
                $s->execute();
                if($s->rowCount() > 0){
                    header("http/1.1 201 created");
                    echo json_encode(array("add" => "Y", "id" => $c->lastInsertId()));
                }else{
                    header("http/1.1 400 bad request");
                    echo json_encode(array("add" => "N"));
                }
            }else{
                header("http/1.1 400 bad request");
                echo "Faltan datos";
            }
        break;
    case 'PUT':
            if(isset($_GET['id'])){
                $sql = "UPDATE registro SET ";
                (isset($_GET['user'])) ? $sql .="user = :us, " : null;
                (isset($_GET['sensor'])) ? $sql .="sensor = :s, " : null;
                (isset($_GET['ubicacion'])) ? $sql .="ubicacion = :ub, " : null;
                (isset($_GET['estado'])) ? $sql .="estado = :e, " : null;
                (isset($_GET['fecha'])) ? $sql .="fecha   = :f, " : null;
                $sql = substr($sql, 0, -2);
                $sql .= " WHERE id = :id";
                $c = conexion();
                $s = $c->prepare($sql);
                (isset($_GET['user'])) ? $s->bindValue(":us", $_GET['user']) : null;
                (isset($_GET['sensor'])) ? $s->bindValue(":s", $_GET['sensor']) : null;
                (isset($_GET['ubicacion'])) ? $s->bindValue(":ub", $_GET['ubicacion']) : null;
                (isset($_GET['estado'])) ? $s->bindValue(":e", $_GET['estado']) : null;
                (isset($_GET['fecha'])) ? $s->bindValue(":f", $_GET['fecha']) : null;
                $s->bindValue(":id", $_GET['id']);
                $s->execute();
                if($s->rowCount() > 0){
                    header("http/1.1 200 ok");
                    echo json_encode(array("update" => "Y"));
                }else{
                    header("http/1.1 400 bad request");
                    echo json_encode(array("update" => "N"));
                }
            }else{
                header("http/1.1 400 bad Request");
            }
        break;
    case 'DELETE':
        if(isset($_GET['id'])){
            $c = conexion();
            $s = $c->prepare("DELETE FROM registro WHERE id = :id");
            $s->bindValue(":id", $_GET['id']);
            $s->execute();
            if($s->rowCount() > 0){
                header("http/1.1 200 ok");
                echo json_encode(array("delete" => "Y"));
            }else{
                header("http/1.1 400 bad request");
                echo json_encode(array("delete" => "N"));
            }
        }else{
            header("http/1.1 400 bad request");
            echo "Faltan datos";
        }
        break;
    default:
        header("http/1.1 405 Method Not Allowed");
}