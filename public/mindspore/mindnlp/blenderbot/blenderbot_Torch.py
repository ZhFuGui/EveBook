import os
import torch
from transformers import BlenderbotTokenizer, BlenderbotForConditionalGeneration, TrainingArguments, Trainer, TrainerCallback
from datasets import load_dataset

# 设置同步 CUDA 调用
os.environ["CUDA_LAUNCH_BLOCKING"] = "1"
os.makedirs("./results", exist_ok=True)

# 加载数据集并过滤空值
dataset = load_dataset("databricks/databricks-dolly-15k")
dataset = dataset.filter(lambda x: x["instruction"] is not None and x["response"] is not None)

# 加载模型和 tokenizer
model_name = "facebook/blenderbot-400M-distill"
tokenizer = BlenderbotTokenizer.from_pretrained(model_name)
model = BlenderbotForConditionalGeneration.from_pretrained(model_name)
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model.to(device)

# 在训练前保存未微调的模型
untrained_model_path = "./untrained_blenderbot"
os.makedirs(untrained_model_path, exist_ok=True)
model.save_pretrained(untrained_model_path)
tokenizer.save_pretrained(untrained_model_path)
print(f"未微调的模型已保存到 {untrained_model_path}")

# 数据预处理
def preprocess_function(examples):
    inputs = [str(i) if i is not None else "" for i in examples["instruction"]]
    targets = [str(t) if t is not None else "" for t in examples["response"]]
    model_inputs = tokenizer(inputs, max_length=128, truncation=True, padding="max_length")
    with tokenizer.as_target_tokenizer():
        labels = tokenizer(targets, max_length=128, truncation=True, padding="max_length")
    model_inputs["labels"] = labels["input_ids"]
    return model_inputs

tokenized_dataset = dataset.map(preprocess_function, batched=True, remove_columns=dataset["train"].column_names)

# 检查数据
for i in range(5):
    print(f"样本 {i} 的 input_ids 长度: {len(tokenized_dataset['train'][i]['input_ids'])}")

# 自定义回调函数，每 0.1 epoch 保存模型
class SaveEveryTenthEpochCallback(TrainerCallback):
    def __init__(self, save_path_prefix, total_epochs, steps_per_epoch):
        self.save_path_prefix = save_path_prefix
        self.total_epochs = total_epochs
        self.steps_per_epoch = steps_per_epoch
        self.save_interval = 0.1  # 每 0.1 epoch 保存一次

    def on_step_end(self, args, state, control, **kwargs):
        current_epoch = state.epoch  # 当前 epoch（浮点数）
        if current_epoch > 0 and state.global_step % int(self.steps_per_epoch * self.save_interval) == 0:
            save_path = f"{self.save_path_prefix}/epoch_{current_epoch:.1f}"
            kwargs["model"].save_pretrained(save_path)
            tokenizer.save_pretrained(save_path)
            print(f"模型已保存到 {save_path} (Epoch {current_epoch:.1f})")

# 设置训练参数
training_args = TrainingArguments(
    output_dir="./results",
    num_train_epochs=3,
    per_device_train_batch_size=1,
    gradient_accumulation_steps=1,
    learning_rate=2e-5,
    fp16=False,
    save_steps=500,  # 保留默认保存策略（可选）
    save_total_limit=2,  # 限制默认检查点数量
    logging_steps=10,
    report_to="none",
)

# 计算每个 epoch 的步数
steps_per_epoch = len(tokenized_dataset["train"]) // (training_args.per_device_train_batch_size * training_args.gradient_accumulation_steps)

# 初始化 Trainer 并添加回调
trainer = Trainer(
    model=model,
    args=training_args,
    train_dataset=tokenized_dataset["train"],
    eval_dataset=tokenized_dataset["validation"] if "validation" in tokenized_dataset else None,
    callbacks=[SaveEveryTenthEpochCallback("./fine_tuned_blenderbot_checkpoints", training_args.num_train_epochs, steps_per_epoch)]
)

# 训练模型
trainer.train()

# 保存最终微调模型
fine_tuned_model_path = "./fine_tuned_blenderbot"
model.save_pretrained(fine_tuned_model_path)
tokenizer.save_pretrained(fine_tuned_model_path)
print(f"最终微调模型已保存到 {fine_tuned_model_path}")