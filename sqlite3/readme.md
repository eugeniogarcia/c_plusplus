# Sqlite

Mas instrucciones [aqui](https://sqlite.org/cli.html). 

Abrimos la base de datos:

```ps
sqlite3 test.db
```

Creamos una tabla:

```ps
create table tbl1(one varchar(10), two smallint);
```

Insertamos algunos datos:

```ps
insert into tbl1 values('hello!',10);
insert into tbl1 values('goodbye', 20);
```

Los consultamos:

```ps
select * from tbl1;
```

Podemos cambiar como los datos se presentan:

- Lista

```ps
.mode list
```

- Especificar el separador

```ps
.separator ", "
```

- Poner quotes en los strings

```ps
.mode quote
```

- Otros modos

```ps
.mode line
.mode column
```

- quitar las cabeceras:

```ps
.headers off
```

## Usar el programa

Podemos usar nuestro programa como 

```ps
sqlite3 test.db "select * from tbl1"
```