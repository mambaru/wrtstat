# wrtstat — документация

Библиотека общего назначения на C++ для сбора и агрегации числовых метрик: временные окна, lossy-сэмплирование, перцентили (p50–p100). Не привязана к конкретному фреймворку или приложению.

## Назначение

wrtstat собирает потоки числовых метрик (латентность, размеры, произвольные значения), группирует их по временным окнам, применяет [lossy-сэмплирование](#lossy-сэмплирование) и вычисляет перцентили (p50, p80, p95, p99, p100).

**Основной способ интеграции** — фабрики `create_*_meter`, которые выдают RAII-объекты (`time_point`, `value_point`, …). Их создают **до** постановки задачи в очередь и захватывают в лямбду отложенного вызова; при завершении работы (уничтожении point) метрика автоматически попадает в агрегатор. Для `time_meter` это даёт время от момента создания point до конца выполнения — **включая ожидание в очереди**, а не только тело обработчика.

## Метры — основной сценарий

### Фабрика и point

| Фабрика | Point | Что записывается при уничтожении point |
|---------|-------|----------------------------------------|
| `create_time_meter<D>()` | `time_point<D>` | Интервал `steady_clock` от создания до `~time_point` |
| `create_value_meter()` | `value_point` | Заданное значение и count |
| `create_size_meter()` | `size_point` | Размер payload |
| `create_composite_meter<D>()` | `composite_point<D>` | Время + объёмы read/write |

Фабрику (`time_meter`, `value_meter`, …) создают один раз и переиспользуют; для каждого измерения вызывают `create(count)` или `create_shared(...)`.

Point отправляет данные в деструкторе (`push()` → агрегатор). Явный вызов `push()` возможен, но обычно не нужен.

### Отложенный вызов и очередь задач

Типичный паттерн — измерить end-to-end латентность обработки, когда задача проходит через thread pool / actor / callback-очередь:

```cpp
#include <wrtstat/wrtstat.hpp>
#include <functional>

wrtstat::wrtstat stat(opt);
auto id = stat.create_aggregator("handler_latency", 0);
auto time_meter = stat.create_time_meter<std::chrono::microseconds>(id);

void enqueue(std::function<void()> job);

// point создаётся ДО постановки в очередь — в интервал войдёт ожидание
auto meter = time_meter.create_shared(1);
enqueue([meter = std::move(meter)]() noexcept {
  handle_request();  // meter уничтожается здесь → sample в агрегатор
});
```

Для `time_point` отсчёт идёт с момента **конструктора**, а не входа в лямбду. Поэтому point нужно создавать **до** `enqueue`/`post`/`dispatch`, если в метрику должно попасть время ожидания в очереди.

`time_point` не копируется — для захвата в лямбду используйте `create_shared()` (или `std::make_shared` + `clone()`). Синхронный код может обойтись `create()` и move.

Полный рабочий пример: [`examples/example4.cpp`](../examples/example4.cpp) — `time_point` передаётся в `std::function` и срабатывает после «отложенного» выполнения.

### Синхронное измерение блока кода

```cpp
{
  auto point = time_meter.create(1);
  do_work();  // ~point → push() с длительностью do_work()
}
```

### Дополнительные типы метров

- **`value_meter`** — фиксированное значение в конце scope (код ответа, число ошибок). Point можно передать в отложенный callback так же через `create_shared`.
- **`size_meter`** — размер сообщения/ответа в байтах.
- **`composite_meter`** — время плюс read/write size за одну операцию; размеры можно обновить перед уничтожением (`set_read_size`, `set_write_size`). См. `tests/wrtstat_suite.cpp`.

Префиксные варианты `create_*_multi_meter` создают несколько одноимённых метрик с разными префиксами (шардинг, несколько инстансов сервиса).

## Быстрый старт

```cpp
#include <wrtstat/wrtstat.hpp>
#include <chrono>

wrtstat::wrtstat::options_type opt;
opt.resolution = wrtstat::resolutions::microseconds;
opt.aggregation_step_ts = 1000000; // окно 1 секунда

wrtstat::wrtstat stat(opt);
auto id = stat.create_aggregator("latency", 0);
auto time_meter = stat.create_time_meter<std::chrono::microseconds>(id);

{
  auto point = time_meter.create(1);
  // ... работа ...
}  // метрика уже в агрегаторе

if (auto ag = stat.force_pop(id))
  // ag->perc50, ag->perc100, ag->count, ...
```

Другие примеры: `examples/example4.cpp` (очередь + лямбда), `examples/example9.cpp` (handler + pushout).

## Интеграция

### Подключение в CMake-проект

```cmake
# вариант 1: как подмодуль / add_subdirectory
add_subdirectory(external/wrtstat)
target_link_libraries(my_app PRIVATE wrtstat)

# вариант 2: после установки (make install)
find_package(wrtstat REQUIRED)
target_link_libraries(my_app PRIVATE wrtstat)
```

Зависимости библиотеки: **faslib**, **wjson** (подтягиваются через `target_link_libraries`).

Минимальный код приложения:

```cpp
#include <wrtstat/wrtstat.hpp>

wrtstat::wrtstat stat(/* options */);
auto id = stat.create_aggregator("handler_latency", 0);
auto time_meter = stat.create_time_meter<std::chrono::microseconds>(id);

auto meter = time_meter.create_shared(1);
enqueue([meter = std::move(meter)]() noexcept {
  handle_request();
});

stat.pushout();  // или stat.force_pop(id)
```

### Сценарии использования

| Сценарий | API | Пример |
|----------|-----|--------|
| **Латентность через очередь (основной)** | `create_time_meter` + `create_shared` в лямбде | `examples/example4.cpp` |
| Callback при flush агрегированных окон | `wrtstat` + `opt.handler` | `examples/example9.cpp` |
| Много потоков, запись по имени | `multi_aggregator` | `examples/example8.cpp` |
| Сериализация в JSON | `request::push_json`, `multi_push_json` | `tests/json_suite.cpp` |
| Прямая запись без RAII | `add()` / `create_value_meter` | `examples/example1.cpp` |
| Низкоуровневый контроль | `aggregator` / `aggregator_mt` | `examples/example3.cpp` |

### Жизненный цикл

1. Создать `wrtstat` один раз на процесс/сервис; завести агрегаторы и **фабрики метров** (`create_time_meter` и т.д.).
2. На каждую операцию — `create` / `create_shared` point **до** постановки в очередь; захватить point в лямбду отложенного вызова.
3. Point сам отправит sample при уничтожении; периодически вызывать `pushout()` / `force_pushout()` (или полагаться на flush при уничтожении `wrtstat`, если задан `handler`).
4. Для временного отключения сбора — `enable(false)` без изменения instrumented code.

## Архитектура

```
Meters (RAII) ──► aggregator_registry ──► aggregator_mt
                                              │
                                         basic_aggregator
                                         ├── separator (окна)
                                         └── reducer (lossy)
                                              │
                                         aggregated_data
                                              │
                              handler / pop / pushout / JSON
```

**multi_aggregator** — шардированный реестр для concurrent ingestion по имени метрики.  
**basic_packer** — пакетная упаковка в JSON `multi_push`.

## Lossy-сэмплирование

**Lossy-сэмплирование** (от *lossy* — «с потерями») — способ обработки высокочастотного потока метрик, при котором библиотека **намеренно не хранит каждое значение**, но сохраняет репрезентативную выборку для расчёта перцентилей и агрегатов. Это компромисс между точностью распределения и bounded-памятью: при миллионах samples в секунду хранить всё невозможно, поэтому часть событий отбрасывается.

В wrtstat lossy-сэмплирование работает в **два этапа**:

### 1. Reducer (при приёме samples в окне)

Компонент `reducer` накапливает значения в многоуровневом reservoir:

- **`reducer_limit`** — ёмкость одного уровня (по умолчанию 4096);
- **`reducer_levels`** — число уровней (по умолчанию 256);
- **`reducer_mode`** — алгоритм слияния уровней: `sorting`, `nth` или `adapt` (автовыбор).

Пока reservoir не заполнен, каждое значение сохраняется. Когда все уровни заняты, **новые значения отбрасываются** — счётчик `lossy` увеличивается. При слиянии уровней (`reduce()`) часть старых samples также «схлопывается» в компактную выборку на верхнем уровне. При этом **min**, **max** и **avg** считаются по полному потоку (`count` = общее число событий), а перцентили — по **оставшейся** выборке в `data`.

Если `lossy > 0`, перцентили приближённые; поле `max` в агрегате может не совпадать с истинным p100 всего потока (см. комментарий в `reduced_info`).

### 2. Урезание на выходе (`outgoing_reduced_size`)

Перед `pop()` / handler вектор `aggregated_data::data` может быть дополнительно прорежен до **`outgoing_reduced_size`** элементов (по умолчанию 128; `0` — без урезания). Это второй, независимый этап lossy-сэмплирования — уже на этапе экспорта готового окна, чтобы ограничить размер JSON/передаваемых данных.

### Поля `count` и `lossy`

| Поле | Смысл |
|------|-------|
| `count` | Сколько событий реально произошло (включая отброшенные) |
| `lossy` | Сколько из них **не попало** в сохранённую выборку индивидуально |

Отношение `lossy / count` показывает, насколько агрессивно сработало сэмплирование в данном окне.

## Конфигурация

Иерархия опций (JSON через `load_options`):

| Параметр | По умолчанию | Описание |
|----------|--------------|----------|
| `reducer_limit` | 4096 | Ёмкость одного уровня reservoir (см. [Lossy-сэмплирование](#lossy-сэмплирование)) |
| `reducer_levels` | 256 | Число уровней reservoir |
| `reducer_mode` | adapt | sorting / nth / adapt |
| `aggregation_step_ts` | 1 | Размер временного окна |
| `resolution` | seconds | none / seconds / ms / µs / ns |
| `soiled_start_ts` | 0 | Случайный сдвиг старта окон |
| `outgoing_reduced_size` | 128 | Лимит `data` на выходе; второй этап lossy (0 = без урезания) |
| `pool_size` | 0 | Размер object pool |

Загрузка из файла или строки:

```cpp
wrtstat::wrtstat_options opt;
std::string err;
wrtstat::load_options("config.json", &opt, &err);
```

## API фасада `wrtstat`

| Метод | Описание |
|-------|----------|
| `create_aggregator(name, ts)` | Создать именованный агрегатор |
| `create_*_meter(...)` | **Основной API:** фабрики RAII-метров (time / value / size / composite) |
| `add(id, ts, v, count)` | Прямая запись без point (вспомогательный путь) |
| `pop(id)` / `force_pop(id)` | Извлечь готовое окно |
| `pushout()` / `force_pushout()` | Flush через handler |
| `enable(bool)` / `enabled()` | Включить/выключить сбор |
| `del(name)` | Удалить метрику из реестра |

### Handler callback

```cpp
opt.handler = [](const std::string& name, wrtstat::aggregated_data::ptr ag) noexcept {
  // обработка агрегированного окна
};
```

Handler вызывается из `pushout()` / `force_pushout()`. При уничтожении `wrtstat` вызывается `force_pushout()`; если handler не задан, данные отбрасываются без вызова.

## Режим `enable(false)`

При `enable(false)`:

- `add()` / `push()` **возвращают `true`**, но **данные не сохраняются**
- `pop()` / `force_pop()` / `separate()` **не отключены**
- Проверка состояния: `enabled()` → `false`

Это позволяет временно отключить сбор без ошибок в instrumented code. Для проверки наличия данных используйте `force_pop()` — при disabled вернёт `nullptr`.

## Separator: устаревшие timestamp

`separator::add` возвращает `false`, если `ts < next_time - step_ts`. Такие samples отбрасываются. Timestamp должен попадать в текущее или более новое окно.

## Потокобезопасность

- `aggregator_mt` — mutex на каждый агрегатор
- `aggregator_registry` — rwlock на реестр
- `multi_aggregator` — шардирование по hash имени (4096 бакетов)
- Фасад `wrtstat` — не документирован как полностью thread-safe; для concurrent доступа используйте `multi_aggregator` или внешнюю синхронизацию

## Сборка

Полный список целей: `make help`.

**Основная команда для сборки библиотеки:**

```bash
make release
```

Release-сборка (статическая или shared — по умолчанию cmake-ci), предупреждения отключены (`DISABLE_WARNINGS=ON`).

### Цели Makefile

| Цель | Назначение |
|------|------------|
| **`make release`** | **Основная release-сборка** |
| `make init` | Первичная конфигурация CMake (`cmake -B ./build`) |
| `make static` | Сборка статической библиотеки (`BUILD_SHARED_LIBS=OFF`) |
| `make shared` | Сборка shared-библиотеки (`BUILD_SHARED_LIBS=ON`) |
| `make tests` | Сборка + запуск unit-тестов (`ctest`) |
| `make paranoid` | Сборка с `-Werror` и строгими предупреждениями |
| `make debug` | Debug + дополнительные предупреждения |
| `make coverage` | Сборка с coverage, тесты, summary-отчёт |
| `make coverage-report` | Coverage + HTML-отчёт в `docs/html/cov-report` |
| `make cppcheck` | Статический анализ cppcheck |
| `make doc` | Doxygen → каталог `docs/` |
| `make install` | Установка из `./build` (`make install ARGS="--prefix ..."`) |
| `make clean` | Очистка `build/` и `docs/` |
| `make update` | Обновление submodule cmake-ci |
| `make upgrade` | `update` + upgrade-скрипт |
| `make docker-build` | Docker-образ для сборки |
| `make docker-run` | Запуск контейнера |
| `make docker-rm` | Удаление образа |
| `make help` | Список целей и примеры |

### Примеры

```bash
make release                    # обычная сборка
make static                     # только .a
make shared VERBOSE=1 ARGS=-j8  # shared, 8 потоков
make tests                      # тесты
make paranoid                   # CI-строгость
make install ARGS="--prefix ./build/test-install"
```

Перед первой сборкой обычно достаточно `make init`, затем **`make release`** или `make tests`.

## Ссылки

- [Doxygen](https://mambaru.github.io/wrtstat/index.html)
- [Coverage report](https://mambaru.github.io/wrtstat/cov-report/index.html)
- [GitHub](https://github.com/mambaru/wrtstat)
