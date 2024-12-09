
<script setup lang="ts">
import {
  DialogContent,
  DialogDescription,
  DialogFooter,
  DialogHeader,
  DialogTitle,
  DialogClose
} from '@/components/ui/dialog'
import { AlertDialogTrigger, AlertDialog } from '@/components/ui/alert-dialog'
import AlertConfirm from '@/components/AlertConfirm.vue';
import { Button } from '@/components/ui/button'
import { Input } from '@/components/ui/input';
import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from '@/components/ui/select'
import { Trash2 } from 'lucide-vue-next'
import { ref } from 'vue';


defineProps<{
    tableName: string,
    cols?: {name:string, type:string}[]
    }>()
    
const column = ref<{name:string, type:string}[]>([]);

async function deleteTable() {
    console.log("test");
}
</script>

<template>

    <DialogContent>
        <DialogHeader>
            <DialogTitle>Edit the table {{ tableName }}</DialogTitle>
            <DialogDescription>
                
            </DialogDescription>
        </DialogHeader>

        <div class="flex items-center" v-for="col of column">
            <Input type="text" name="tableName" placeholder="Table Name"  v-model="col.name"/>
            <Select v-model="col.type">
                <SelectTrigger>
                <SelectValue placeholder="Column type" />
                </SelectTrigger>
                <SelectContent>
                    <SelectItem value="apple">Apple</SelectItem>
                    <SelectItem value="apple">Apple</SelectItem>
                    <SelectItem value="apple">Apple</SelectItem>
                    <SelectItem value="apple">Apple</SelectItem>
                    <SelectItem value="apple">Apple</SelectItem>
                    <SelectItem value="apple">Apple</SelectItem>
                </SelectContent>
            </Select>
            <Button variant="destructive" @click="column.splice(column.indexOf(col), 1)">
                <Trash2 />
            </Button>
        </div>

        <Button variant="outline" @click="column.push({name:'', type:''})">Add Column</Button>

        <AlertDialog>
        <AlertDialogTrigger>
            <Button variant="destructive">
                <Trash2 />
            </Button>
            <AlertConfirm :description="`This action will delete the table ${tableName}`" :action="deleteTable"></AlertConfirm>
        </AlertDialogTrigger>
    </AlertDialog>

        <DialogFooter>
            <DialogClose as-child>
                <Button type="button" variant="secondary">
                    Cancel
                </Button>
            </DialogClose>
            <Button @click="">
                Save Changes
            </Button>
        </DialogFooter>
    </DialogContent>
</template>