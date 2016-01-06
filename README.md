# img2web
Micro-service of online image microblogs

--------
Dependencies
===============
1. cassandra.so
2. libuv.so
3. all deb-packages fastcgi-daemon
Собирать через make

img2web REST API:

INTERFACE
===============
<b>REQUEST</b>
<ul>
    <li>URL: /feed</li>
    <li>METHOD: GET</li>
    <li>PARAMS: page_number, login</li>
    <li>
    Примечание: page_number - некое число 0 .... N - пользователю будут показаны изображения в промежутке:
        (page_number * 10, page_number * 10 + 10) // 10 изображений на страницу</li>
</ul>
<b>RESPONSE</b>
<ul>
    <li>Content-Type: application/json</li>
    <li>Code: Success - 200 OK</li>
    <li>Body if success:
        {[{"login" : login1, image_binary_format" : image_binary_format1},
          {"login" : login2, image_binary_format" : image_binary_format2}... ]}</li>
</ul>
--------
<b>REQUEST</b>
<ul>
    <li>URL: /users  // возвращает логины всех, когда либо логинившихся пользователей</li>
    <li>METHOD: GET</li>

</ul>
<b>RESPONSE</b>
<ul>
    <li>Content-Type: application/json</li>
    <li>Code: Success - 200</li>
    <li>Body if success:
        {[{"login": usr_login}, ... ]}</li>
</ul>
--------
<b>REQUEST</b>
<ul>
    <li>URL: /subscribe // добавлять в ленту пользователя с логином login записи пользователя username</li>
    <li>METHOD: POST</li>
    <li>PARAMS: login, username</li>
</ul>
<b>RESPONSE</b>
<ul>
    <li>Content-Type: plain/text</li>
    <li>Code: Success - 200 FAIL - 434 // login or username does not exist </li>
</ul>
--------
<b>REQUEST</b>
<ul>
    <li>URL: /post</li>
    <li>METHOD: POST</li>
    <li>PARAMS: login, image_binary_format</li>
</ul>
<b>RESPONSE</b>
<ul>
    <li>Content-Type: application/json</li>
    <li>Code: Success - 200 FAIL - 434 Incorrect login</li>
</ul>
--------
CAP
===============
Про CAP теорему:
Для системы обмена картинками консистентность не столь важна как доступность. Поэтому в первую очередь это AP система. В связи с такой направленностью была выбрана Cassandra - в стандартной конфигурации это AP система с приятными бонусами в духе Partiotion Key (позволяет задавать способ разбиения изображений по кластерам, например, с привязкой по географии). Особенностью Cassandra являются быстрые операции вставки (вот поэтому в неё часто пишут логи) - т.е. вставка картинок (особенно с учётом реализации структуры бд) - довольно эффективная операция.
Т.к. мы хотим, чтобы нашим сервисом могли пользоваться всегда (вне зависимости от падения машинок) и чтобы при разделении сети (или падении машинки) сохранялись все функции (посмотреть фото / загрузить фото) мы приходим к системе с равноправными узлами (каждый хранит свою часть информации и каждый может отвечать на запросы). Это автоматически даёт нам A и P. При разделении сети / проблемами с машинкой, однако, пользователи не смогут увидеть части фоточек - каких именно определяется Partition Key (логично сделать с привязкой к региону / времени, но у меня сделано чуть проще - PK являяется логин)
Насчёт Consistency в Cassandra. Она обладает широкими возможностями по настройке того, нужна ли нам Strong Consistency (тогда это CP система) или же Eventual Consistency. Эти настройки раздельны для чтения и записи.
Настраивается также и replication фактор (и стратегия репликации).
Опираясь на эту презентацию http://www.slideshare.net/DataStax/understanding-data-consistency-in-apache-cassandra - (слайд 8) - для обеспечения Durability и EC я бы выбрал стратегию с чтением / записью в кворум (т.е. как минимум replication_factor + 1 / 2 реплик с данной колонкой должны быть доступны). Мы жертвуем теоретический availability - но зато более менее гарантируем адекватность информации в репликах.
Всё рассмотренное - в пределах одного датацентра, при дальнейшем масштабировании надо думать
Итого - почти полностью A, полностью P и довольно неплохой C - кажется, неплохой трейдофф
