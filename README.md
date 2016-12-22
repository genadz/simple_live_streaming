# сервис для просмотра/публикации аудиозаписей

Используется REST API, в первую очередь из-за популярности (сторонним разработчикам будет относительно легко осуществить интеграцию с нашим сервисом) и простоты подхода. Также, REST предоставляет хорошие возможности для масштабируемости (за счет кеширования и отсутствия состояния) и расширяемости (легкое добавление ендпоинтов).

С помощью API можно получать список аудиозаписей, лайкать, загружать, скачивать аудиозаписи.

Запросы

## GET /songs
получить список песен.

параметры запроса:
- from_date - с какого периода (время в unixtime, по умолчанию 0)
- to_date - по какой период (время в unixtime, по умолчанию текущее время)
- limit - максимальное количество возвращаемых песен (по умолчанию 0, т.е. все песни за данный период)
```
{
  "links" : {
    "self": /songs,
    "best": /songs/best,
    "new": /songs/new,
  }
  "songs": [
    {
      "name": name,
      "author": author,
      "likes": 1,
      "upload_date": "17-12-2016",
      "duration": 180,
      "format": mp3,
      "links": {
        "self": /songs/id,
        "download": /audiofiles/id
        "upload": /audiofiles/id
        "like": /songs/id/like
        }
    },
    {
      ...
    }
  ]
}
```

## GET /songs/best
получить список лучших песен (идут в отсортированном по полю "likes" порядке) 

параметры такие же, как и в запросе GET /songs
```
{
  "links": {"self": /songs/best}
  "songs": [
    {
      ...
    }
  ]
}
```

## GET /songs/new
получить список новых песен (идут в отсортированном по полю "upload_date" порядке)

параметры такие же, как и в запросе GET /songs
```
{
  "links": {"self": /songs/new}
  "songs": [
    {
      ...
    }
  ]
}
```

## GET /songs/{id}
получить описание песни

пример ответа:
```
{
  "name": name,
  "author": author,
  "likes": 1,
  "upload_date": "20-12-2016",
  "duration": 666,
  "format": mp3,
  "links": {
    "self": /songs/id,
    "download": /audiofiles/id
    "upload": /audiofiles/id
    "like": /songs/id/like
    }
}
```

## POST /songs
создать новую песню

пример тела запроса
```
{
  "name": name,
  "author": author,
  "duration": 200,
  "format": flac
}
```

в ответе получаем описание песни в формате json, в поле "upload" будет лежать урл, по которому можно загрузить саму песню (в формате mp3, flac,...)
```
{
  "name": name,
  "author": author,
  "duration": 200,
  "likes": 0,
  "format": flac
  "links": {
    "self": /songs/id,
    "download": /audiofiles/id
    "upload": /audiofiles/id
    "like": /songs/id/like
  }
}
```

## PUT /songs/{id}

Для изменения информации о песнях, используется метод PUT

пример тела запроса:
```
{ "name": name2 }
```

в ответе возвращается описание песни в формате json

## PUT /audiofiles/{id}
загрузить аудиофайл, в теле запроса содержится файл в формате "format" . Ответ пустой

## GET /audiofiles/{id}
скачать аудиофайл, тело запроса пустое; в ответе получаем аудиофайл

## POST /songs/{id}/like
поставить лайк песне. тело запроса и ответ пустые
