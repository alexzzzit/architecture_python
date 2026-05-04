# Проектирование документной модели для MongoDB
## Вариант 20: Система управления медицинскими записями

### Коллекции и структура документов

#### users
{
  _id: ObjectId,
  id: NumberLong,
  login: String,
  email: String,
  password_hash: String,
  role: String,
  is_active: Boolean,
  created_at: ISODate,
  updated_at: ISODate
}

#### patients
{
  _id: ObjectId,
  id: NumberLong,
  user_id: NumberLong,
  policy_number: String,
  snils: String,
  first_name: String,
  last_name: String,
  middle_name: String,
  birth_date: ISODate,
  phone: String,
  address: {
    city: String,
    street: String,
    building: String,
    apartment: String
  },
  emergency_contact: {
    name: String,
    phone: String,
    relation: String
  },
  created_at: ISODate,
  updated_at: ISODate
}

#### medical_records
{
  _id: ObjectId,
  id: NumberLong,
  code: String,
  patient_id: NumberLong,
  doctor_id: NumberLong,
  diagnosis: String,
  symptoms: [String],
  treatment_plan: {
    medications: [{
      name: String,
      dosage: String,
      frequency: String
    }],
    procedures: [String],
    recommendations: String
  },
  attachments: [{
    filename: String,
    url: String,
    uploaded_at: ISODate
  }],
  status: String,
  visit_date: ISODate,
  created_at: ISODate,
  updated_at: ISODate
}

#### counters
{
  _id: String,
  seq: NumberLong
}

### Обоснование: Embedded vs References

| Связь | Решение | Обоснование |
|-------|---------|-------------|
| users <-> patients | Reference | 1:1 связь, но сущности имеют независимые жизненные циклы и разные сценарии поиска |
| patients <-> medical_records | Reference | 1:N, записи часто запрашиваются пагинацией по дате, отдельная коллекция позволяет эффективно индексировать |
| medical_records.treatment_plan | Embedded | Данные всегда читаются вместе с записью, нет необходимости в отдельных запросах |
| medical_records.attachments | Embedded | Метаданные файлов логично хранить внутри документа записи |

### Индексы

db.users.createIndex({ id: 1 }, { unique: true })
db.users.createIndex({ login: 1 }, { unique: true })
db.users.createIndex({ email: 1 }, { unique: true })
db.users.createIndex({ last_name: "text", first_name: "text" })

db.patients.createIndex({ id: 1 }, { unique: true })
db.patients.createIndex({ user_id: 1 }, { unique: true })
db.patients.createIndex({ policy_number: 1 }, { unique: true })
db.patients.createIndex({ snils: 1 }, { unique: true })
db.patients.createIndex({ last_name: 1, first_name: 1 })

db.medical_records.createIndex({ id: 1 }, { unique: true })
db.medical_records.createIndex({ code: 1 }, { unique: true })
db.medical_records.createIndex({ patient_id: 1, visit_date: -1 })
db.medical_records.createIndex({ doctor_id: 1, visit_date: -1 })
db.medical_records.createIndex({ diagnosis: "text" })

db.counters.createIndex({ _id: 1 }, { unique: true })
