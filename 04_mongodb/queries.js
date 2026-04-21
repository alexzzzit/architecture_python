db = db.getSiblingDB('medical_db');

// CREATE
db.users.insertOne({
  id: NumberLong(100),
  login: "new_patient",
  email: "new@example.com",
  password_hash: "$2b$10$test",
  role: "patient",
  is_active: true,
  created_at: new Date(),
  updated_at: new Date()
});

db.patients.insertOne({
  id: NumberLong(100),
  user_id: NumberLong(100),
  policy_number: "9999999999999999",
  snils: "111-222-333 44",
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

db.medical_records.insertOne({
  id: NumberLong(100),
  code: "REC-2024-TEST",
  patient_id: NumberLong(100),
  doctor_id: NumberLong(1),
  diagnosis: "Тестовая запись",
  symptoms: ["симптом1"],
  treatment_plan: { medications: [], procedures: [], recommendations: "Наблюдение" },
  attachments: [],
  status: "draft",
  visit_date: new Date(),
  created_at: new Date(),
  updated_at: new Date()
});

// READ
db.users.findOne({ login: { $eq: "doctor_smirnov" } });

db.users.find({
  $or: [
    { last_name: { $regex: "иван", $options: "i" } },
    { first_name: { $regex: "иван", $options: "i" } }
  ]
});

db.patients.find({
  last_name: "Иванов",
  first_name: { $in: ["Иван", "Игорь"] }
});

db.medical_records.find({
  patient_id: NumberLong(1),
  status: { $ne: "archived" }
}).sort({ visit_date: -1 });

db.medical_records.findOne({ code: "REC-2024-001" });

db.medical_records.find({
  $text: { $search: "бронхит" }
});

db.medical_records.find({
  patient_id: NumberLong(1),
  visit_date: {
    $gte: new Date("2024-01-01"),
    $lte: new Date("2024-12-31")
  }
});

// UPDATE
db.medical_records.updateOne(
  { code: "REC-2024-TEST" },
  {
    $set: {
      status: "confirmed",
      updated_at: new Date()
    }
  }
);

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

db.medical_records.updateOne(
  { code: "REC-2024-001" },
  {
    $addToSet: {
      attachments: {
        filename: "blood_test_20240305.pdf",
        url: "s3://bucket/records/001/blood.pdf",
        uploaded_at: new Date("2024-03-05")
      }
    }
  }
);

db.patients.updateOne(
  { policy_number: "1234567890123456" },
  { $set: { phone: "+79009998877", updated_at: new Date() } }
);

// DELETE
db.medical_records.deleteOne({
  code: "REC-2024-TEST",
  status: "draft"
});

db.users.updateOne(
  { login: "new_patient" },
  { $set: { is_active: false, updated_at: new Date() } }
);

const patient = db.patients.findOne({ policy_number: "9999999999999999" });
if (patient) {
  db.medical_records.deleteMany({ patient_id: patient.id });
  db.patients.deleteOne({ _id: patient._id });
  db.users.deleteOne({ id: patient.user_id });
}

print("CRUD операции выполнены");
