// queries.js — CRUD операции для варианта 20 (медицинские записи)
// Запуск: mongosh "mongodb://localhost:27017/medical_db" < queries.js

db = db.getSiblingDB('medical_db');

print("\n=== CREATE (Вставка) ===");

// 1. Создание нового пользователя
db.users.insertOne({
  id: NumberLong(100),
  login: "new_patient_test",
  email: "newpatient@test.com",
  password_hash: "$2b$10$testhash123",
  role: "patient",
  is_active: true,
  created_at: new Date(),
  updated_at: new Date()
});
print("Создан пользователь: new_patient_test");

// 2. Регистрация пациента
db.patients.insertOne({
  id: NumberLong(100),
  user_id: NumberLong(100),
  policy_number: "9999888877776666",
  snils: "999-888-777 66",
  first_name: "Тест",
  last_name: "Тестов",
  middle_name: "Тестович",
  birth_date: new Date("1990-01-01"),
  phone: "+79990001122",
  address: { city: "Москва", street: "Ленина", building: "1", apartment: "1" },
  emergency_contact: { name: "Родственник", phone: "+79990003344", relation: "брат" },
  created_at: new Date(),
  updated_at: new Date()
});
print("Зарегистрирован пациент: Тестов Т.Т.");

// 3. Создание медицинской записи
db.medical_records.insertOne({
  id: NumberLong(100),
  code: "REC-2024-TEST",
  patient_id: NumberLong(100),
  doctor_id: NumberLong(1),
  diagnosis: "Тестовая запись для демонстрации CRUD",
  symptoms: ["симптом1", "симптом2"],
  treatment_plan: {
    medications: [],
    procedures: [],
    recommendations: "Наблюдение"
  },
  attachments: [],
  status: "draft",
  visit_date: new Date(),
  created_at: new Date(),
  updated_at: new Date()
});
print("Создана медицинская запись: REC-2024-TEST");


print("\n=== READ (Поиск) ===");

// 4. Поиск пользователя по логину ($eq)
print("\n4. Поиск по логину ($eq):");
printjson(db.users.findOne({ login: { $eq: "doctor_smirnov" } }));

// 5. Поиск пользователя по маске имени/фамилии ($regex, case-insensitive)
print("\n5. Поиск по маске ФИО ($regex):");
printjson(db.users.find({
  $or: [
    { last_name: { $regex: "иван", $options: "i" } },
    { first_name: { $regex: "иван", $options: "i" } }
  ]
}).toArray());

// 6. Поиск пациента по ФИО с оператором $in
print("\n6. Поиск пациентов с именем Иван или Игорь ($in):");
printjson(db.patients.find({
  last_name: "Иванов",
  first_name: { $in: ["Иван", "Игорь"] }
}).toArray());

// 7. Получение истории записей пациента (сортировка по дате, $ne)
print("\n7. История записей пациента (статус != archived):");
printjson(db.medical_records.find({
  patient_id: NumberLong(1),
  status: { $ne: "archived" }
}).sort({ visit_date: -1 }).limit(5).toArray());

// 8. Получение записи по коду
print("\n8. Поиск записи по коду:");
printjson(db.medical_records.findOne({ code: "REC-2024-001" }));

// 9. Поиск записей по диагнозу ($text поиск — требуется text index)
print("\n9. Поиск по диагнозу (текстовый поиск):");
printjson(db.medical_records.find({
  $text: { $search: "бронхит" }
}).toArray());

// 10. Записи за период ($gte, $lte)
print("\n10. Записи за период 2024 года ($gte/$lte):");
printjson(db.medical_records.find({
  patient_id: NumberLong(1),
  visit_date: {
    $gte: new Date("2024-01-01"),
    $lte: new Date("2024-12-31")
  }
}).sort({ visit_date: -1 }).toArray());

// 11. Поиск пациентов по городу (вложенный объект)
print("\n11. Пациенты из Москвы:");
printjson(db.patients.find({
  "address.city": "Москва"
}).toArray());

// 12. Поиск записей с определёнными симптомами ($all)
print("\n12. Записи с симптомами 'кашель' и 'температура' ($all):");
printjson(db.medical_records.find({
  symptoms: { $all: ["кашель", "температура"] }
}).toArray());


print("\n=== UPDATE (Обновление) ===");

// 13. Обновление статуса записи ($set)
db.medical_records.updateOne(
  { code: "REC-2024-TEST" },
  {
    $set: {
      status: "confirmed",
      updated_at: new Date()
    }
  }
);
print("Обновлён статус записи REC-2024-TEST: draft -> confirmed");

// 14. Добавление лекарства в план лечения ($push)
db.medical_records.updateOne(
  { code: "REC-2024-001" },
  {
    $push: {
      "treatment_plan.medications": {
        name: "Витамин С",
        dosage: "100мг",
        frequency: "1 раз в день"
      }
    },
    $set: { updated_at: new Date() }
  }
);
print("Добавлено лекарство в REC-2024-001");

// 15. Добавление вложения ($addToSet — избегание дубликатов)
db.medical_records.updateOne(
  { code: "REC-2024-001" },
  {
    $addToSet: {
      attachments: {
        filename: "blood_test_20240305.pdf",
        url: "s3://bucket/records/001/blood.pdf",
        uploaded_at: new Date("2024-03-05T10:00:00Z")
      }
    }
  }
);
print("Добавлено вложение в REC-2024-001 (если не было)");

// 16. Обновление телефона пациента
db.patients.updateOne(
  { policy_number: "1234567890123456" },
  {
    $set: {
      phone: "+79009998877",
      updated_at: new Date()
    }
  }
);
print("Обновлён телефон пациента с полисом 1234567890123456");

// 17. Добавление нового симптома в запись ($push + $each)
db.medical_records.updateOne(
  { code: "REC-2024-003" },
  {
    $push: {
      symptoms: { $each: ["усталость", "головокружение"] }
    },
    $set: { updated_at: new Date() }
  }
);
print("Добавлены симптомы в REC-2024-003");

// 18. Удаление лекарства из плана ($pull)
db.medical_records.updateOne(
  { code: "REC-2024-001" },
  {
    $pull: {
      "treatment_plan.medications": { name: "Парацетамол" }
    },
    $set: { updated_at: new Date() }
  }
);
print("Удалён Парацетамол из плана лечения REC-2024-001");


print("\n=== DELETE (Удаление) ===");

// 19. Удаление черновика записи
db.medical_records.deleteOne({
  code: "REC-2024-TEST",
  status: "draft"
});
print("Удалена черновая запись REC-2024-TEST");

// 20. Деактивация пользователя (мягкое удаление через $set)
db.users.updateOne(
  { login: "new_patient_test" },
  {
    $set: {
      is_active: false,
      updated_at: new Date()
    }
  }
);
print("Деактивирован пользователь new_patient_test");

// 21. Удаление пациента и всех его записей (каскад вручную)
const patientToDelete = db.patients.findOne({ policy_number: "9999888877776666" });
if (patientToDelete) {
  db.medical_records.deleteMany({ patient_id: patientToDelete.id });
  db.patients.deleteOne({ _id: patientToDelete._id });
  db.users.deleteOne({ id: patientToDelete.user_id });
  print("Удалён тестовый пациент и связанные данные");
}

// 22. Удаление всех неактивных пользователей
const deletedCount = db.users.deleteMany({ is_active: false }).deletedCount;
print("Удалено неактивных пользователей: " + deletedCount);


print("\n=== ДОПОЛНИТЕЛЬНЫЕ ОПЕРАТОРЫ ===");

// 23. Поиск записей с количеством симптомов > 2 ($gt)
print("\n23. Записи с более чем 2 симптомами ($gt):");
printjson(db.medical_records.find({
  symptoms: { $expr: { $gt: [{ $size: "$symptoms" }, 2] } }
}).toArray());

// 24. Поиск пациентов, родившихся до 1990 года ($lt)
print("\n24. Пациенты, родившиеся до 1990 года ($lt):");
printjson(db.patients.find({
  birth_date: { $lt: new Date("1990-01-01") }
}).toArray());

// 25. Поиск записей со статусом confirmed или draft ($or)
print("\n25. Активные записи (статус confirmed или draft) ($or):");
printjson(db.medical_records.find({
  $or: [
    { status: "confirmed" },
    { status: "draft" }
  ]
}).limit(5).toArray());

// 26. Обновление нескольких полей одновременно ($set с несколькими полями)
db.patients.updateOne(
  { snils: "123-456-789 00" },
  {
    $set: {
      phone: "+79001234567",
      "address.apartment": "25а",
      updated_at: new Date()
    }
  }
);
print("Обновлены несколько полей пациента одновременно");

print("\nВсе CRUD операции выполнены успешно!");