// validation.js — Валидация схем MongoDB через $jsonSchema
// Запуск: mongosh "mongodb://localhost:27017/medical_db" < validation.js

db = db.getSiblingDB('medical_db');

print("\n=== Настройка валидации для коллекции medical_records ===");

// Удаляем коллекцию если существует (для повторного запуска скрипта)
if (db.medical_records) {
  db.medical_records.drop();
  print("Коллекция medical_records удалена для пересоздания с валидацией");
}

// Создаём коллекцию с валидацией схемы
db.createCollection("medical_records", {
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["id", "code", "patient_id", "doctor_id", "diagnosis", "status", "visit_date"],
      properties: {
        id: {
          bsonType: "long",
          description: "Уникальный бизнес-идентификатор записи (NumberLong)"
        },
        code: {
          bsonType: "string",
          pattern: "^REC-\\d{4}-\\d{3,}$",
          description: "Код записи в формате REC-YYYY-NNN (например, REC-2024-001)"
        },
        patient_id: {
          bsonType: "long",
          description: "ID пациента (ссылка на patients.id)"
        },
        doctor_id: {
          bsonType: "long",
          description: "ID врача (ссылка на users.id)"
        },
        diagnosis: {
          bsonType: "string",
          minLength: 5,
          maxLength: 500,
          description: "Текст диагноза (5-500 символов)"
        },
        symptoms: {
          bsonType: "array",
          items: {
            bsonType: "string",
            minLength: 2
          },
          description: "Список симптомов (каждый минимум 2 символа)"
        },
        treatment_plan: {
          bsonType: ["object", "null"],
          properties: {
            medications: {
              bsonType: "array",
              items: {
                bsonType: "object",
                required: ["name", "dosage", "frequency"],
                properties: {
                  name: { bsonType: "string", description: "Название препарата" },
                  dosage: { bsonType: "string", description: "Дозировка" },
                  frequency: { bsonType: "string", description: "Частота приёма" }
                }
              }
            },
            procedures: {
              bsonType: "array",
              items: { bsonType: "string" }
            },
            recommendations: {
              bsonType: "string"
            }
          }
        },
        attachments: {
          bsonType: "array",
          items: {
            bsonType: "object",
            required: ["filename", "url", "uploaded_at"],
            properties: {
              filename: { bsonType: "string" },
              url: {
                bsonType: "string",
                pattern: "^s3://.*",
                description: "URL должен начинаться с s3://"
              },
              uploaded_at: { bsonType: "date" }
            }
          }
        },
        status: {
          enum: ["draft", "confirmed", "archived"],
          description: "Статус записи: draft | confirmed | archived"
        },
        visit_date: {
          bsonType: "date",
          description: "Дата и время визита"
        },
        created_at: { bsonType: "date" },
        updated_at: { bsonType: "date" }
      }
    }
  },
  validationLevel: "strict",
  validationAction: "error"
});

print("Коллекция medical_records создана с валидацией $jsonSchema");


print("\n=== Тестирование валидации ===");

// ТЕСТ 1: Валидная запись (должна пройти)
print("\n[Тест 1] Валидная запись:");
try {
  db.medical_records.insertOne({
    id: NumberLong(999),
    code: "REC-2024-999",
    patient_id: NumberLong(1),
    doctor_id: NumberLong(1),
    diagnosis: "Тестовый диагноз для проверки валидации схемы",
    symptoms: ["симптом1", "симптом2"],
    treatment_plan: {
      medications: [{ name: "Тест", dosage: "1мг", frequency: "1 раз" }],
      procedures: [],
      recommendations: "Наблюдение"
    },
    attachments: [],
    status: "draft",
    visit_date: new Date(),
    created_at: new Date(),
    updated_at: new Date()
  });
  print("PASS: Валидная запись принята");
} catch (e) {
  print("FAIL: Валидная запись отклонена: " + e.message);
}

// ТЕСТ 2: Невалидный код (не соответствует паттерну)
print("\n[Тест 2] Невалидный код (не соответствует паттерну ^REC-\\d{4}-\\d{3,}$):");
try {
  db.medical_records.insertOne({
    id: NumberLong(998),
    code: "INVALID-CODE",
    patient_id: NumberLong(1),
    doctor_id: NumberLong(1),
    diagnosis: "Тестовый диагноз",
    status: "draft",
    visit_date: new Date()
  });
  print("FAIL: Невалидный код не отловлен");
} catch (e) {
  print("PASS: Невалидный код отклонён: " + e.message.substring(0, 100) + "...");
}

// ТЕСТ 3: Отсутствие обязательного поля (patient_id)
print("\n[Тест 3] Отсутствует обязательное поле patient_id:");
try {
  db.medical_records.insertOne({
    id: NumberLong(997),
    code: "REC-2024-997",
    doctor_id: NumberLong(1),
    diagnosis: "Тестовый диагноз",
    status: "confirmed",
    visit_date: new Date()
  });
  print("FAIL: Отсутствует patient_id, но запись принята");
} catch (e) {
  print("PASS: Отсутствует patient_id, запись отклонена: " + e.message.substring(0, 100) + "...");
}

// ТЕСТ 4: Недопустимое значение enum для status
print("\n[Тест 4] Недопустимое значение status (не из enum):");
try {
  db.medical_records.insertOne({
    id: NumberLong(996),
    code: "REC-2024-996",
    patient_id: NumberLong(1),
    doctor_id: NumberLong(1),
    diagnosis: "Тестовый диагноз",
    status: "invalid_status",
    visit_date: new Date()
  });
  print("FAIL: Невалидный статус не отловлен");
} catch (e) {
  print("PASS: Невалидный статус отклонён: " + e.message.substring(0, 100) + "...");
}

// ТЕСТ 5: Неверный тип данных (diagnosis — число вместо строки)
print("\n[Тест 5] Неверный тип данных для diagnosis (число вместо строки):");
try {
  db.medical_records.insertOne({
    id: NumberLong(995),
    code: "REC-2024-995",
    patient_id: NumberLong(1),
    doctor_id: NumberLong(1),
    diagnosis: 12345,
    status: "draft",
    visit_date: new Date()
  });
  print("FAIL: Неверный тип diagnosis не отловлен");
} catch (e) {
  print("PASS: Неверный тип diagnosis отклонён: " + e.message.substring(0, 100) + "...");
}

// ТЕСТ 6: URL вложения не начинается с s3://
print("\n[Тест 6] URL вложения не соответствует паттерну ^s3://:");
try {
  db.medical_records.insertOne({
    id: NumberLong(994),
    code: "REC-2024-994",
    patient_id: NumberLong(1),
    doctor_id: NumberLong(1),
    diagnosis: "Тестовый диагноз",
    status: "draft",
    visit_date: new Date(),
    attachments: [{
      filename: "test.pdf",
      url: "http://example.com/test.pdf",
      uploaded_at: new Date()
    }]
  });
  print("FAIL: Невалидный URL вложения не отловлен");
} catch (e) {
  print("PASS: Невалидный URL вложения отклонён: " + e.message.substring(0, 100) + "...");
}

// ТЕСТ 7: Слишком короткий диагноз (< 5 символов)
print("\n[Тест 7] Слишком короткий диагноз (minLength: 5):");
try {
  db.medical_records.insertOne({
    id: NumberLong(993),
    code: "REC-2024-993",
    patient_id: NumberLong(1),
    doctor_id: NumberLong(1),
    diagnosis: "ОРЗ",
    status: "draft",
    visit_date: new Date()
  });
  print("FAIL: Короткий диагноз не отловлен");
} catch (e) {
  print("PASS: Короткий диагноз отклонён: " + e.message.substring(0, 100) + "...");
}


print("\n=== Проверка текущей валидации коллекции ===");
const collInfo = db.getCollectionInfos({ name: "medical_records" })[0];
if (collInfo && collInfo.options && collInfo.options.validator) {
  print("Валидация активна для medical_records");
  print("  validationLevel: " + collInfo.options.validationLevel);
  print("  validationAction: " + collInfo.options.validationAction);
} else {
  print("Предупреждение: Валидация не найдена");
}

print("\nВалидация схем настроена и протестирована!");