db = db.getSiblingDB('medical_db');

db.medical_records.drop();

db.createCollection("medical_records", {
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["id", "code", "patient_id", "doctor_id", "diagnosis", "status", "visit_date"],
      properties: {
        id: {
          bsonType: "long",
          description: "Бизнес-ключ записи"
        },
        code: {
          bsonType: "string",
          pattern: "^REC-\\d{4}-\\d{3,}$",
          description: "Код записи в формате REC-YYYY-NNN"
        },
        patient_id: {
          bsonType: "long",
          description: "Ссылка на пациента"
        },
        doctor_id: {
          bsonType: "long",
          description: "Ссылка на врача"
        },
        diagnosis: {
          bsonType: "string",
          minLength: 5,
          maxLength: 500,
          description: "Диагноз (5-500 символов)"
        },
        symptoms: {
          bsonType: "array",
          items: {
            bsonType: "string",
            minLength: 2
          },
          description: "Список симптомов"
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
                  name: { bsonType: "string" },
                  dosage: { bsonType: "string" },
                  frequency: { bsonType: "string" }
                }
              }
            },
            procedures: { bsonType: "array", items: { bsonType: "string" } },
            recommendations: { bsonType: "string" }
          }
        },
        attachments: {
          bsonType: "array",
          items: {
            bsonType: "object",
            required: ["filename", "url", "uploaded_at"],
            properties: {
              filename: { bsonType: "string" },
              url: { bsonType: "string", pattern: "^s3://.*" },
              uploaded_at: { bsonType: "date" }
            }
          }
        },
        status: {
          enum: ["draft", "confirmed", "archived"],
          description: "Статус записи"
        },
        visit_date: {
          bsonType: "date",
          description: "Дата визита"
        },
        created_at: { bsonType: "date" },
        updated_at: { bsonType: "date" }
      }
    }
  },
  validationLevel: "strict",
  validationAction: "error"
});

print("Тестирование валидации:");

try {
  db.medical_records.insertOne({
    id: NumberLong(999),
    code: "REC-2024-999",
    patient_id: NumberLong(1),
    doctor_id: NumberLong(1),
    diagnosis: "Тестовый диагноз для валидации",
    status: "draft",
    visit_date: new Date(),
    created_at: new Date(),
    updated_at: new Date()
  });
  print("OK: Валидная запись принята");
} catch (e) {
  print("ERROR: Валидная запись отклонена: " + e.message);
}

try {
  db.medical_records.insertOne({
    id: NumberLong(998),
    code: "INVALID-CODE",
    patient_id: NumberLong(1),
    doctor_id: NumberLong(1),
    diagnosis: "Тест",
    status: "draft",
    visit_date: new Date()
  });
  print("ERROR: Невалидный код не отловлен");
} catch (e) {
  print("OK: Невалидный код отклонён: " + e.message);
}

try {
  db.medical_records.insertOne({
    id: NumberLong(997),
    code: "REC-2024-997",
    doctor_id: NumberLong(1),
    diagnosis: "Тест",
    status: "confirmed",
    visit_date: new Date()
  });
  print("ERROR: Отсутствует patient_id, но запись принята");
} catch (e) {
  print("OK: Отсутствует patient_id, запись отклонена: " + e.message);
}

print("Валидация схем настроена и протестирована");
