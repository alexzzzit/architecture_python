db = db.getSiblingDB('medical_db');

db.users.deleteMany({});
db.patients.deleteMany({});
db.medical_records.deleteMany({});
db.counters.deleteMany({});

db.counters.insertMany([
  { _id: "users", seq: 0 },
  { _id: "patients", seq: 0 },
  { _id: "medical_records", seq: 0 }
]);

function getNextId(collectionName) {
  const result = db.counters.findAndModify({
    query: { _id: collectionName },
    update: { $inc: { seq: 1 } },
    new: true
  });
  return result.seq;
}

const users = [
  {
    id: NumberLong(getNextId("users")),
    login: "doctor_smirnov",
    email: "smirnov@hospital.ru",
    password_hash: "$2b$10$abc123",
    role: "doctor",
    is_active: true,
    created_at: new Date("2024-01-15"),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("users")),
    login: "patient_ivanov",
    email: "ivanov@mail.ru",
    password_hash: "$2b$10$def456",
    role: "patient",
    is_active: true,
    created_at: new Date("2024-02-01"),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("users")),
    login: "patient_petrova",
    email: "petrova@example.com",
    password_hash: "$2b$10$ghi789",
    role: "patient",
    is_active: true,
    created_at: new Date("2024-02-10"),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("users")),
    login: "admin_root",
    email: "admin@hospital.ru",
    password_hash: "$2b$10$jkl012",
    role: "admin",
    is_active: true,
    created_at: new Date("2024-01-01"),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("users")),
    login: "lab_kozlov",
    email: "kozlov@lab.ru",
    password_hash: "$2b$10$mno345",
    role: "lab_technician",
    is_active: true,
    created_at: new Date("2024-01-20"),
    updated_at: new Date()
  }
];
db.users.insertMany(users);

const patients = [
  {
    id: NumberLong(getNextId("patients")),
    user_id: NumberLong(2),
    policy_number: "1234567890123456",
    snils: "123-456-789 00",
    first_name: "Иван",
    last_name: "Иванов",
    middle_name: "Иванович",
    birth_date: new Date("1985-06-15"),
    phone: "+79001234567",
    address: {
      city: "Москва",
      street: "Тверская",
      building: "10",
      apartment: "25"
    },
    emergency_contact: {
      name: "Мария Иванова",
      phone: "+79007654321",
      relation: "супруга"
    },
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("patients")),
    user_id: NumberLong(3),
    policy_number: "6543210987654321",
    snils: "987-654-321 00",
    first_name: "Анна",
    last_name: "Петрова",
    middle_name: "Сергеевна",
    birth_date: new Date("1992-03-22"),
    phone: "+79009876543",
    address: {
      city: "Санкт-Петербург",
      street: "Невский проспект",
      building: "28",
      apartment: "12"
    },
    emergency_contact: {
      name: "Пётр Петров",
      phone: "+79001112233",
      relation: "супруг"
    },
    created_at: new Date(),
    updated_at: new Date()
  }
];
db.patients.insertMany(patients);

const records = [
  {
    id: NumberLong(getNextId("medical_records")),
    code: "REC-2024-001",
    patient_id: NumberLong(1),
    doctor_id: NumberLong(1),
    diagnosis: "Острый бронхит",
    symptoms: ["кашель", "температура 37.8", "слабость"],
    treatment_plan: {
      medications: [
        { name: "Амброксол", dosage: "30мг", frequency: "3 раза в день" },
        { name: "Парацетамол", dosage: "500мг", frequency: "при температуре" }
      ],
      procedures: ["ингаляции", "постельный режим"],
      recommendations: "Обильное тёплое питьё, избегать переохлаждения"
    },
    attachments: [
      {
        filename: "xray_chest_20240301.jpg",
        url: "s3://bucket/records/001/xray.jpg",
        uploaded_at: new Date("2024-03-01")
      }
    ],
    status: "confirmed",
    visit_date: new Date("2024-03-01T10:30:00Z"),
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("medical_records")),
    code: "REC-2024-002",
    patient_id: NumberLong(1),
    doctor_id: NumberLong(1),
    diagnosis: "Профилактический осмотр",
    symptoms: [],
    treatment_plan: {
      medications: [],
      procedures: ["анализ крови", "ЭКГ"],
      recommendations: "Повторный визит через 6 месяцев"
    },
    attachments: [],
    status: "confirmed",
    visit_date: new Date("2024-06-15T14:00:00Z"),
    created_at: new Date(),
    updated_at: new Date()
  },
  {
    id: NumberLong(getNextId("medical_records")),
    code: "REC-2024-003",
    patient_id: NumberLong(2),
    doctor_id: NumberLong(1),
    diagnosis: "Гипертония 1 степени",
    symptoms: ["повышенное давление", "головная боль"],
    treatment_plan: {
      medications: [
        { name: "Лизиноприл", dosage: "10мг", frequency: "1 раз в день утром" }
      ],
      procedures: ["мониторинг АД"],
      recommendations: "Ограничение соли, контроль веса"
    },
    attachments: [],
    status: "confirmed",
    visit_date: new Date("2024-04-10T09:15:00Z"),
    created_at: new Date(),
    updated_at: new Date()
  }
];
db.medical_records.insertMany(records);

print("Тестовые данные загружены успешно");
