# Qv2ray 1.3.5 错误更新回滚方法
## ROLLBACK METHOD IF YOU HAVE UPDATED TO THE v1.3.4 VERSION

# 如果你已经更新到了 1.3.5，请在使用旧版 Qv2ray 之前进行以下操作
## IF YOU HAVE ALREADY UPDATED TO 1.3.5, PLEASE DO THESE **BEFORE** OPENING OLD VERSION!

- Linux & MacOS: 

打开  `$HOME/.qv2ray/Qv2ray.conf`

OPEN `$HOME/.qv2ray/Qv2ray.conf`

- Windows: 

打开  `C:\Users\你的用户名\.qv2ray\Qv2ray.conf`

OPEN `C:\Users\YOUR_USERNAME\.qv2ray\Qv2ray.conf`

# 把 `config_version` 后的 `2` 修改成 `"1"`
## CHANGE `config_version` FROM `2` TO `"1"`

修改之前 BEFORE CHANGE：

![image](https://user-images.githubusercontent.com/18734999/64166267-555c7c80-ce79-11e9-9128-ad932a36c35d.png)

修改之后 AFTER CHANGE：

![image](https://user-images.githubusercontent.com/18734999/64166308-63120200-ce79-11e9-9727-323ec3e5c123.png)

# 然后重新启动旧版本，并忽略此次更新
## THEN START THE OLD VERSION AND IGNORE THIS UPDATE IF POSSIBLE
